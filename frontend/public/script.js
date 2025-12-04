// API Configuration
// Local: use localhost:5000
// Deployed: use 192.168.2.12:5000 (your local machine IP)
const API_BASE = (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1')
  ? 'http://localhost:5000/api'
  : 'http://192.168.2.12:5000/api';

// DOM Elements
const codeEditor = document.getElementById('code');
const compileBtn = document.getElementById('compileBtn');
const runBtn = document.getElementById('runBtn');
const clearBtn = document.getElementById('clearBtn');
const exampleSelect = document.getElementById('exampleSelect');
const errorsContainer = document.getElementById('errors');
const symbolsContainer = document.getElementById('symbols');
const outputContainer = document.getElementById('output');
const consoleOutput = document.getElementById('consoleOutput');
const consoleInput = document.getElementById('consoleInput');
const errorCountSpan = document.getElementById('errorCount');
const symbolCountSpan = document.getElementById('symbolCount');
const tabButtons = document.querySelectorAll('.tab');
const tabContents = document.querySelectorAll('.tab-content');

// Runtime state
let runtime = null;
let inputResolver = null;
let lastCompiledCode = '';
let examples = {};

// Event Listeners
compileBtn.addEventListener('click', compileCode);
runBtn.addEventListener('click', runCode);
clearBtn.addEventListener('click', clearCode);
exampleSelect.addEventListener('change', loadExample);
consoleInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter' && !consoleInput.disabled) handleInput();
});
tabButtons.forEach(btn => {
    btn.addEventListener('click', () => switchTab(btn.dataset.tab));
});

// Initialize
loadExamples();
loadDefaultExample();

async function loadExamples() {
    try {
        const response = await fetch(`${API_BASE}/examples`);
        examples = await response.json();
        
        Object.entries(examples).forEach(([key, example]) => {
            const option = document.createElement('option');
            option.value = key;
            option.textContent = example.name;
            exampleSelect.appendChild(option);
        });
    } catch (error) {
        console.error('Error loading examples:', error);
    }
}

function loadDefaultExample() {
    const defaultCode = `nexus {
    shard core x, y;
    listen x;
    listen y;
    shard core sum = x + y;
    broadcast sum;
}`;
    codeEditor.value = defaultCode;
}

function loadExample() {
    const key = exampleSelect.value;
    if (key && examples[key]) {
        codeEditor.value = examples[key].code;
    }
}

function clearCode() {
    codeEditor.value = '';
    errorsContainer.innerHTML = '';
    symbolsContainer.innerHTML = '';
    outputContainer.innerHTML = '';
    consoleOutput.innerHTML = '';
    errorCountSpan.textContent = '0';
    symbolCountSpan.textContent = '0';
}

function switchTab(tabName) {
    tabButtons.forEach(btn => {
        btn.classList.toggle('active', btn.dataset.tab === tabName);
    });
    tabContents.forEach(content => {
        content.classList.toggle('active', content.id === tabName);
    });
}

async function compileCode() {
    const code = codeEditor.value.trim();
    if (!code) {
        alert('Please enter some code first');
        return;
    }

    compileBtn.disabled = true;
    compileBtn.textContent = 'Compiling...';

    try {
        const response = await fetch(`${API_BASE}/compile`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code })
        });

        const result = await response.json();
        lastCompiledCode = code;
        
        displayErrors(result.errors || []);
        displaySymbols(result.symbolTable || {});
        
        runBtn.disabled = result.hasErrors;
        
        switchTab('errors');
    } catch (error) {
        console.error('Compilation error:', error);
        alert('Failed to compile: ' + error.message);
    } finally {
        compileBtn.disabled = false;
        compileBtn.textContent = 'Compile';
    }
}

function displayErrors(errors) {
    errorCountSpan.textContent = errors.length;
    
    if (errors.length === 0) {
        errorsContainer.innerHTML = '<div class="output-line system">✓ No errors found</div>';
        return;
    }

    errorsContainer.innerHTML = '';
    errors.forEach(err => {
        const errorDiv = document.createElement('div');
        errorDiv.className = 'error-item';
        errorDiv.innerHTML = `
            <div class="error-type">${err.type}</div>
            <div class="error-message">${err.message}</div>
            <div class="error-location">Line ${err.line}, Column ${err.column}</div>
        `;
        errorsContainer.appendChild(errorDiv);
    });
}

function displaySymbols(symbolTable) {
    const symbols = Object.values(symbolTable);
    symbolCountSpan.textContent = symbols.length;
    
    if (symbols.length === 0) {
        symbolsContainer.innerHTML = '<div class="output-line system">No symbols declared</div>';
        return;
    }

    symbolsContainer.innerHTML = '';
    symbols.forEach(symbol => {
        const symbolDiv = document.createElement('div');
        symbolDiv.className = 'symbol-item';
        symbolDiv.innerHTML = `
            <span class="symbol-name">${symbol.name}</span>
            <span class="symbol-type">${symbol.type}</span>
            <div class="symbol-location">Line ${symbol.line}, Column ${symbol.column}</div>
        `;
        symbolsContainer.appendChild(symbolDiv);
    });
}

async function runCode() {
    if (!lastCompiledCode) {
        alert('Please compile the code first');
        return;
    }

    switchTab('output');
    consoleOutput.innerHTML = '<div class="output-line system">▶ Starting program...</div>';
    consoleInput.disabled = false;
    consoleInput.placeholder = 'Waiting for program input...';

    runtime = new CodeFlowRuntime(
        (message, type) => displayOutput(message, type),
        (varName) => requestInput(varName)
    );

    try {
        await runtime.execute(lastCompiledCode);
    } catch (error) {
        displayOutput(`\nRUNTIME ERROR: ${error.message}`, 'error');
        
        errorsContainer.innerHTML += `
            <div class="error-item">
                <div class="error-type">RUNTIME</div>
                <div class="error-message">${error.message}</div>
            </div>
        `;
        errorCountSpan.textContent = parseInt(errorCountSpan.textContent) + 1;
        switchTab('errors');
    }

    consoleInput.disabled = true;
    consoleInput.placeholder = 'Program finished';
}

function displayOutput(message, type = 'output') {
    const line = document.createElement('div');
    line.className = `output-line ${type}`;
    line.textContent = message;
    consoleOutput.appendChild(line);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

function requestInput(varName) {
    return new Promise((resolve) => {
        displayOutput(`Enter value for '${varName}': `, 'prompt');
        consoleInput.disabled = false;
        consoleInput.focus();
        inputResolver = resolve;
    });
}

function handleInput() {
    if (!inputResolver) return;
    
    const value = consoleInput.value.trim();
    displayOutput(`> ${value}`, 'input');
    
    inputResolver(value);
    inputResolver = null;
    
    consoleInput.value = '';
    consoleInput.disabled = true;
}

function showInfo() {
    const infoTab = document.getElementById('info');
    infoTab.innerHTML = `
        <h3>59LANG Language</h3>
        <p><strong>Version:</strong> 1.0</p>
        <p><strong>Description:</strong> A cyberpunk-inspired programming language with static typing.</p>
        
        <h4>Syntax (Old):</h4>
        <ul>
            <li><code>main { }</code> - Program entry point</li>
            <li><code>var int x;</code> - Variable declaration</li>
            <li><code>input x;</code> - Read input</li>
            <li><code>output x;</code> - Print output</li>
        </ul>

        <h4>Syntax (New):</h4>
        <ul>
            <li><code>nexus { }</code> - Program entry point</li>
            <li><code>shard core x;</code> - Declare int variable</li>
            <li><code>listen x;</code> - Read input</li>
            <li><code>broadcast x;</code> - Print output</li>
            <li><code>probe (x > 0) { }</code> - If statement</li>
            <li><code>fallback { }</code> - Else clause</li>
            <li><code>pulse (x > 0) { }</code> - While loop</li>
        </ul>

        <h4>Example:</h4>
        <code>nexus {<br>
        &nbsp;&nbsp;shard core x, y;<br>
        &nbsp;&nbsp;listen x;<br>
        &nbsp;&nbsp;listen y;<br>
        &nbsp;&nbsp;shard core sum = x + y;<br>
        &nbsp;&nbsp;broadcast sum;<br>
        }</code>
    `;
}
