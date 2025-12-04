from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import json
import os
import tempfile
import sys

app = Flask(__name__)
CORS(app)

# Path to the compiled compiler executable
# Try multiple locations: artifact (from CI), build directory, current directory
COMPILER_PATHS = [
    os.path.join(os.path.dirname(__file__), '..', 'artifact', 'compiler'),
    os.path.join(os.path.dirname(__file__), '..', 'artifact', 'compiler.exe'),
    os.path.join(os.path.dirname(__file__), '..', 'build', 'Release', 'compiler.exe'),
    os.path.join(os.path.dirname(__file__), '..', 'build', 'compiler.exe'),
    os.path.join(os.path.dirname(__file__), '..', 'build', 'compiler'),
    'compiler',
    'compiler.exe'
]

COMPILER_PATH = None
for path in COMPILER_PATHS:
    if os.path.exists(path):
        COMPILER_PATH = path
        break

@app.route('/api/health', methods=['GET'])
def health():
    """Health check endpoint"""
    return jsonify({
        'status': 'ok',
        'compiler_available': os.path.exists(COMPILER_PATH),
        'language': '59LANG'
    })

@app.route('/api/compile', methods=['POST'])
def compile_code():
    """
    Compile 59LANG source code
    Request body:
    {
        "code": "source code",
        "filename": "optional filename"
    }
    """
    try:
        data = request.get_json()
        
        if not data or 'code' not in data:
            return jsonify({
                'error': 'Missing "code" field in request body'
            }), 400
        
        source_code = data['code']
        filename = data.get('filename', 'unnamed.code')
        
        # Check if compiler exists
        if not os.path.exists(COMPILER_PATH):
            return jsonify({
                'error': f'Compiler not found at {COMPILER_PATH}. Please build the C++ compiler first.'
            }), 500
        
        # Create temporary file with source code
        with tempfile.NamedTemporaryFile(mode='w', suffix='.code', delete=False) as f:
            f.write(source_code)
            temp_file = f.name
        
        try:
            # Run compiler with JSON output
            result = subprocess.run(
                [COMPILER_PATH, temp_file, '--json'],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            # Parse JSON output
            output = json.loads(result.stdout)
            
            return jsonify({
                'success': not output.get('hasErrors', False),
                'errors': output.get('errors', []),
                'symbolTable': output.get('symbolTable', {}),
                'errorCount': output.get('errorCount', 0),
                'hasErrors': output.get('hasErrors', False),
                'tokens': output.get('tokens', []),
                'ast': output.get('ast', {})
            })
        
        finally:
            # Clean up temporary file
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    except subprocess.TimeoutExpired:
        return jsonify({
            'error': 'Compilation timeout'
        }), 408
    
    except json.JSONDecodeError:
        return jsonify({
            'error': 'Invalid JSON output from compiler'
        }), 500
    
    except Exception as e:
        return jsonify({
            'error': str(e)
        }), 500

@app.route('/api/examples', methods=['GET'])
def get_examples():
    """Get example programs"""
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
    
    return jsonify(examples)

@app.route('/', methods=['GET'])
def index():
    """Serve static files"""
    return app.send_static_file('index.html')

if __name__ == '__main__':
    app.run(debug=True, port=5000)
