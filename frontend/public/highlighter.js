// 59LANG Syntax Highlighter
// Real-time token detection and coloring

class CodeHighlighter {
    constructor(textareaId, highlightContainerId) {
        this.textarea = document.getElementById(textareaId);
        this.highlightContainer = document.getElementById(highlightContainerId);
        this.keywords = new Set([
            'nexus', 'main', 'shard', 'core', 'flux', 'sig', 'glyph',
            'listen', 'broadcast', 'input', 'output',
            'probe', 'fallback', 'if', 'else',
            'pulse', 'cycle', 'while', 'for',
            'var', 'const', 'return', 'func',
            'and', 'or', 'not', 'true', 'false',
            'int', 'float', 'bool', 'string',
            'do', 'break', 'continue', 'switch', 'case', 'default', 'void'
        ]);
        
        this.setup();
    }
    
    setup() {
        // Sync scroll between textarea and highlight container
        this.textarea.addEventListener('scroll', () => {
            this.highlightContainer.scrollTop = this.textarea.scrollTop;
            this.highlightContainer.scrollLeft = this.textarea.scrollLeft;
        });
        
        // Highlight on input
        this.textarea.addEventListener('input', () => this.highlight());
        
        // Initial highlight
        this.highlight();
    }
    
    highlight() {
        const code = this.textarea.value;
        const html = this.tokenize(code);
        this.highlightContainer.innerHTML = html;
    }
    
    tokenize(code) {
        let html = '';
        let i = 0;
        
        while (i < code.length) {
            // Whitespace and newlines
            if (/\s/.test(code[i])) {
                html += code[i];
                i++;
                continue;
            }
            
            // String literals (double quotes)
            if (code[i] === '"') {
                let str = '"';
                i++;
                while (i < code.length && code[i] !== '"') {
                    if (code[i] === '\\') {
                        str += code[i] + code[i + 1];
                        i += 2;
                    } else {
                        str += code[i];
                        i++;
                    }
                }
                if (i < code.length) str += code[i++];
                html += `<span class="highlight-string">${this.escapeHtml(str)}</span>`;
                continue;
            }
            
            // Comments (%)
            if (code[i] === '%') {
                let comment = '';
                while (i < code.length && code[i] !== '\n') {
                    comment += code[i++];
                }
                html += `<span class="highlight-comment">${this.escapeHtml(comment)}</span>`;
                continue;
            }
            
            // Numbers
            if (/\d/.test(code[i])) {
                let num = '';
                while (i < code.length && /[\d.]/.test(code[i])) {
                    num += code[i++];
                }
                html += `<span class="highlight-number">${num}</span>`;
                continue;
            }
            
            // Identifiers and keywords
            if (/[a-zA-Z_]/.test(code[i])) {
                let word = '';
                while (i < code.length && /[a-zA-Z0-9_]/.test(code[i])) {
                    word += code[i++];
                }
                if (this.keywords.has(word.toLowerCase())) {
                    html += `<span class="highlight-keyword">${word}</span>`;
                } else {
                    html += `<span class="highlight-identifier">${word}</span>`;
                }
                continue;
            }
            
            // Operators and punctuation
            if (code[i] === ';' || code[i] === ',') {
                // Red: semicolons and commas
                html += `<span class="highlight-semicolon">${code[i]}</span>`;
                i++;
                continue;
            }
            
            if (code[i] === '(' || code[i] === ')' || code[i] === '{' || code[i] === '}' || 
                code[i] === '[' || code[i] === ']') {
                // Blue: parentheses and braces
                html += `<span class="highlight-paren">${code[i]}</span>`;
                i++;
                continue;
            }
            
            if (code[i] === '+' || code[i] === '-' || code[i] === '*' || code[i] === '/' || 
                code[i] === '%' || code[i] === '=' || code[i] === '<' || code[i] === '>' || 
                code[i] === '!' || code[i] === '&' || code[i] === '|' || code[i] === '^' ||
                code[i] === '?' || code[i] === ':') {
                // Check for multi-char operators
                let op = code[i];
                if (i + 1 < code.length) {
                    const twoChar = code[i] + code[i + 1];
                    if (['++', '--', '==', '!=', '<=', '>=', '&&', '||', '<<', '>>', '**', '->'].includes(twoChar)) {
                        op = twoChar;
                        i++;
                    }
                }
                html += `<span class="highlight-operator">${this.escapeHtml(op)}</span>`;
                i++;
                continue;
            }
            
            // Default: just output the character
            html += this.escapeHtml(code[i]);
            i++;
        }
        
        return html;
    }
    
    escapeHtml(text) {
        const map = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };
        return text.replace(/[&<>"']/g, m => map[m]);
    }
}

// Initialize on page load
let codeHighlighterInstance = null;
document.addEventListener('DOMContentLoaded', () => {
    codeHighlighterInstance = new CodeHighlighter('code', 'highlighter');
});

// Export for external use
window.updateHighlight = () => {
    if (codeHighlighterInstance) {
        codeHighlighterInstance.highlight();
    }
};
