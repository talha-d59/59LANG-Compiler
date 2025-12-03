// Simple CodeFlow Runtime Simulator
class CodeFlowRuntime {
    constructor(outputCallback, inputCallback) {
        this.outputCallback = outputCallback;
        this.inputCallback = inputCallback;
        this.variables = {};
        this.waiting = false;
    }

    async execute(code) {
        this.variables = {};
        this.waiting = false;
        
        try {
            // Tokenize: ensure braces become standalone lines for reliable block parsing
            const normalized = code
                .replace(/\{/g, '\n{\n')
                .replace(/\}/g, '\n}\n');
            const lines = normalized
                .split('\n')
                .map(l => l.trim())
                .filter(l => l.length > 0);

            // Remove only the leading program header line (nexus/main) and the first '{' and final '}'
            const cleaned = [];
            let removedOpening = false;
            let pendingSkipClosingIndex = -1;
            for (let i = 0; i < lines.length; i++) {
                const line = lines[i];
                if (!removedOpening && (line === 'nexus' || line === 'main')) {
                    removedOpening = true;
                    continue;
                }
                if (!removedOpening && (line.startsWith('nexus ') || line.startsWith('main '))) {
                    removedOpening = true; // header variants
                    continue;
                }
                if (removedOpening === true && !removedOpening && line === '{') {
                    removedOpening = true;
                    continue;
                }
                cleaned.push(line);
            }
            // If last line is a solitary '}', drop it
            if (cleaned.length && cleaned[cleaned.length - 1] === '}') cleaned.pop();

            // Execute with block-aware traversal
            for (let i = 0; i < cleaned.length && !this.waiting; i++) {
                const line = cleaned[i].replace(/;$/, '').trim();

                // If/Probe block
                if (line.startsWith('if') || line.startsWith('probe')) {
                    const { thenBlock, elseBlock, nextIndex } = this.collectIfElseBlocks(cleaned, i);
                    const cond = this.extractCondition(line);
                    const condResult = this.evaluateCondition(cond);
                    if (condResult) {
                        await this.executeBlock(thenBlock);
                    } else if (elseBlock.length > 0) {
                        await this.executeBlock(elseBlock);
                    }
                    i = nextIndex - 1; // skip to after blocks
                    continue;
                }

                // While/Pulse block
                if (line.startsWith('while') || line.startsWith('pulse')) {
                    const { block, condition, nextIndex } = this.collectWhileBlock(cleaned, i);
                    while (this.evaluateCondition(condition)) {
                        await this.executeBlock(block);
                        if (this.waiting) break; // if waiting for input, pause
                    }
                    i = nextIndex - 1;
                    continue;
                }

                // Regular single-line execution
                await this.executeLine(line);
            }
            
            if (!this.waiting) {
                this.outputCallback('\n✓ Program finished', 'system');
            }
        } catch (error) {
            this.outputCallback(`Error: ${error.message}`, 'error');
        }
    }

    async executeLine(line) {
        line = line.replace(/;$/, '').trim();
        
        // Variable declaration: var int x, y; OR shard core x, y; OR shard core sum = x + y;
        if (line.startsWith('var ') || line.startsWith('shard ')) {
            const match = line.match(/(?:var|shard)\s+(\w+)\s+([\w,\s=+\-*/().'"]+)/);
            if (match) {
                let type = match[1];
                // Map new keywords to internal types
                if (type === 'core') type = 'int';
                if (type === 'flux') type = 'float';
                if (type === 'sig') type = 'bool';
                if (type === 'glyph') type = 'string';
                
                // Handle declarations with or without initialization
                const declParts = match[2].split(',').map(v => v.trim());
                declParts.forEach(decl => {
                    if (decl.includes('=')) {
                        // Declaration with initialization: sum = x + y
                        const [varName, expr] = decl.split('=').map(s => s.trim());
                        this.variables[varName] = { 
                            type, 
                            value: this.evaluateExpression(expr) 
                        };
                    } else {
                        // Plain declaration: x
                        this.variables[decl] = { type, value: null };
                    }
                });
            }
        }
        // Input: input x; OR listen x;
        else if (line.startsWith('input ') || line.startsWith('listen ')) {
            const varName = line.replace(/^(input|listen)\s+/, '').trim();
            const value = await this.inputCallback(varName);
            if (this.variables[varName]) {
                this.variables[varName].value = this.parseValue(value, this.variables[varName].type);
            }
        }
        // Output: output x; OR broadcast x; or output "text"; OR broadcast "text";
        else if (line.startsWith('output ') || line.startsWith('broadcast ')) {
            const expr = line.replace(/^(output|broadcast)\s+/, '').trim();
            const value = this.evaluateExpression(expr);
            this.outputCallback(value, 'output');
        }
        // Assignment: x = 5; or sum = x + y;
        else if (line.includes('=') && !line.includes('==')) {
            const [varName, expr] = line.split('=').map(s => s.trim());
            if (this.variables[varName]) {
                this.variables[varName].value = this.evaluateExpression(expr);
            }
        }
        // Block statements handled in execute() using collect* helpers
    }

    // Execute a sequence of simple lines (no nested blocks inside)
    async executeBlock(blockLines) {
        for (const bLine of blockLines) {
            const line = bLine.replace(/;$/, '').trim();
            if (!line) continue;
            await this.executeLine(line);
            if (this.waiting) break;
        }
    }

    // Extract condition from probe/if/pulse/while header line
    extractCondition(headerLine) {
        const m = headerLine.match(/\((.*)\)/);
        return m ? m[1].trim() : '';
    }

    // Collect { then } [fallback { else }] starting from probe/if line
    collectIfElseBlocks(lines, startIndex) {
        // Expect form: probe (cond) { ... } [fallback { ... }]
        let i = startIndex + 1;
        // Find opening brace for then-block
        while (i < lines.length && lines[i] !== '{') i++;
        if (i >= lines.length) return { thenBlock: [], elseBlock: [], nextIndex: startIndex + 1 };
        const thenRes = this.collectBraceBlock(lines, i);
        const thenBlock = thenRes.block;
        i = thenRes.endIndex + 1;
        // Optional fallback clause
        let elseBlock = [];
        if (i < lines.length && /^fallback\b/.test(lines[i])) {
            i++;
            while (i < lines.length && lines[i] !== '{') i++;
            if (i < lines.length && lines[i] === '{') {
                const elseRes = this.collectBraceBlock(lines, i);
                elseBlock = elseRes.block;
                i = elseRes.endIndex + 1;
            }
        }
        return { thenBlock, elseBlock, nextIndex: i };
    }

    // Collect pulse/while (cond) { ... }
    collectWhileBlock(lines, startIndex) {
        const condition = this.extractCondition(lines[startIndex]);
        let i = startIndex + 1;
        if (lines[i] !== '{') {
            while (i < lines.length && lines[i] !== '{') i++;
        }
        const { block, endIndex } = this.collectBraceBlock(lines, i);
        return { block, condition, nextIndex: endIndex + 1 };
    }

    // Collect lines within matching { ... } starting at index of '{'
    collectBraceBlock(lines, openIndex) {
        // openIndex points to a '{'
        let depth = 0;
        const block = [];
        for (let i = openIndex; i < lines.length; i++) {
            const ln = lines[i].trim();
            if (ln === '{') { depth++; if (depth > 1) { block.push(ln); } continue; }
            if (ln === '}') { depth--; if (depth === 0) { return { block, endIndex: i }; } { block.push(ln); } continue; }
            block.push(ln);
        }
        // Fallback if unmatched
        return { block, endIndex: lines.length - 1 };
    }

    evaluateExpression(expr) {
        expr = expr.trim();
        
        // String literal
        if (expr.startsWith('"') && expr.endsWith('"')) {
            return expr.slice(1, -1);
        }
        
        // Variable reference
        if (this.variables[expr]) {
            return this.variables[expr].value;
        }
        
        // Arithmetic expression: x + y
        if (expr.includes('+') || expr.includes('-') || expr.includes('*') || expr.includes('/')) {
            return this.evaluateArithmetic(expr);
        }
        
        // Numeric literal
        if (!isNaN(expr)) {
            return parseFloat(expr);
        }
        
        return expr;
    }

    evaluateArithmetic(expr) {
        // Simple arithmetic evaluation
        const operators = ['+', '-', '*', '/'];
        for (let op of operators) {
            if (expr.includes(op)) {
                const parts = expr.split(op).map(p => p.trim());
                const left = this.evaluateExpression(parts[0]);
                const right = this.evaluateExpression(parts[1]);
                
                switch(op) {
                    case '+': return Number(left) + Number(right);
                    case '-': return Number(left) - Number(right);
                    case '*': return Number(left) * Number(right);
                    case '/': return Number(left) / Number(right);
                }
            }
        }
        return 0;
    }

    evaluateCondition(condition) {
        // Support logical operators join/either/void, and/or/not
        if (!condition) return false;

        // Replace language operators with JS equivalents for simple eval
        let expr = condition
            .replace(/\bjoin\b/g, '&&')
            .replace(/\beither\b/g, '||')
            .replace(/\bvoid\b/g, '!');

        // Tokenize by spaces and evaluate identifiers and literals
        // Replace identifiers with their numeric/boolean values
        expr = expr.replace(/[A-Za-z_][A-Za-z0-9_]*/g, (name) => {
            if (this.variables[name] !== undefined) {
                const v = this.variables[name].value;
                if (typeof v === 'string') return `'${v}'`;
                return String(v);
            }
            return name; // operators or unknown tokens
        });

        // Now handle comparison operators manually (>=, <=, ==, !=, >, <)
        // For safety, avoid eval for arithmetic: compute via simple parser
        const ops = ['>=', '<=', '==', '!=', '>', '<'];
        for (const op of ops) {
            const idx = expr.indexOf(op);
            if (idx !== -1) {
                const left = expr.slice(0, idx).trim();
                const right = expr.slice(idx + op.length).trim();
                const lv = this.evaluateExpression(left);
                const rv = this.evaluateExpression(right);
                switch (op) {
                    case '>': return lv > rv;
                    case '<': return lv < rv;
                    case '>=': return lv >= rv;
                    case '<=': return lv <= rv;
                    case '==': return lv == rv;
                    case '!=': return lv != rv;
                }
            }
        }

        // If no comparison, evaluate truthiness of expression
        const val = this.evaluateExpression(expr);
        return Boolean(val);
    }

    parseValue(value, type) {
        if (type === 'int') {
            const parsed = parseInt(value);
            if (isNaN(parsed)) {
                throw new Error(`Invalid input: expected integer, got '${value}'`);
            }
            return parsed;
        }
        if (type === 'float') {
            const parsed = parseFloat(value);
            if (isNaN(parsed)) {
                throw new Error(`Invalid input: expected float, got '${value}'`);
            }
            return parsed;
        }
        if (type === 'bool') {
            if (value !== 'true' && value !== 'false' && value !== '0' && value !== '1') {
                throw new Error(`Invalid input: expected boolean (true/false), got '${value}'`);
            }
            return value === 'true' || value === '1';
        }
        return value;
    }
}
