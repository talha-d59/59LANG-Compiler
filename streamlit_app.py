import streamlit as st
import subprocess
import json
import tempfile
import os
from pathlib import Path

st.set_page_config(page_title="59LANG Compiler", layout="wide")

# Title and info
st.title("59LANG Compiler")
st.markdown(
    """
    A compiled language with signal-based control flow. Write clean, expressive code
    with named communication channels and block-aware execution.
    """
)

# Find compiler binary
COMPILER_PATHS = [
    os.path.join(os.path.dirname(__file__), 'build', 'Release', 'compiler.exe'),
    os.path.join(os.path.dirname(__file__), 'build', 'compiler.exe'),
    os.path.join(os.path.dirname(__file__), 'build', 'compiler'),
    'compiler',
    'compiler.exe'
]

COMPILER_PATH = None
for path in COMPILER_PATHS:
    if os.path.exists(path):
        COMPILER_PATH = path
        break

# Sidebar: Examples
st.sidebar.markdown("## Example Programs")

examples = {
    'simple_io': {
        'name': 'Simple I/O',
        'code': '''nexus {
    shard core x, y;
    listen x;
    listen y;
    shard core sum = x + y;
    broadcast sum;
}'''
    },
    'conditional': {
        'name': 'If/Else Statement',
        'code': '''nexus {
    shard core x;
    listen x;
    probe (x > 0) {
        broadcast "Positive";
    } fallback {
        broadcast "Non-positive";
    }
}'''
    },
    'loop': {
        'name': 'While Loop',
        'code': '''nexus {
    shard core i = 1;
    pulse (i <= 5) {
        broadcast i;
        i = i + 1;
    }
}'''
    },
    'factorial': {
        'name': 'Factorial Calculator',
        'code': '''nexus {
    shard core n;
    listen n;
    
    shard core factorial = 1;
    shard core i = 1;
    
    pulse (i <= n) {
        factorial = factorial * i;
        i = i + 1;
    }
    
    broadcast factorial;
}'''
    }
}

# Sidebar: Load example
selected_example = st.sidebar.selectbox("Load Example:", [ex['name'] for ex in examples.values()])
example_code = None
for key, ex in examples.items():
    if ex['name'] == selected_example:
        example_code = ex['code']
        break

# Main content: Editor and Compiler
col1, col2 = st.columns(2)

with col1:
    st.subheader("Source Code")
    source_code = st.text_area(
        "Enter 59LANG source code:",
        value=example_code or "",
        height=300,
        label_visibility="collapsed"
    )

with col2:
    st.subheader("Compilation Results")
    
    if st.button("Compile", key="compile_btn", use_container_width=True):
        if not source_code.strip():
            st.error("Please enter source code to compile.")
        elif not COMPILER_PATH:
            st.error("Compiler binary not found. Please build the C++ compiler first.")
        else:
            try:
                # Create temporary file
                with tempfile.NamedTemporaryFile(mode='w', suffix='.code', delete=False) as f:
                    f.write(source_code)
                    temp_file = f.name
                
                try:
                    # Run compiler
                    result = subprocess.run(
                        [COMPILER_PATH, temp_file, '--json'],
                        capture_output=True,
                        text=True,
                        timeout=5
                    )
                    
                    # Parse output
                    output = json.loads(result.stdout)
                    
                    if output.get('hasErrors', False):
                        st.error(f"Compilation failed ({output.get('errorCount', 0)} errors)")
                        for error in output.get('errors', []):
                            st.write(f"❌ {error}")
                    else:
                        st.success("Compilation successful!")
                        
                        # Show symbol table if available
                        if output.get('symbolTable'):
                            st.markdown("### Symbol Table")
                            st.json(output['symbolTable'])
                
                finally:
                    if os.path.exists(temp_file):
                        os.unlink(temp_file)
            
            except subprocess.TimeoutExpired:
                st.error("Compilation timeout (exceeded 5 seconds).")
            except json.JSONDecodeError:
                st.error("Invalid JSON output from compiler.")
            except Exception as e:
                st.error(f"Error: {str(e)}")

# Footer: Language reference
st.markdown("---")
st.markdown(
    """
    ### 59LANG Keywords
    - **nexus**: entry point, main block
    - **shard**: variable declaration
    - **listen**: input operation
    - **broadcast**: output operation
    - **probe**: if condition (with fallback for else)
    - **pulse**: while loop
    """
)
