#!/usr/bin/env python3
"""Fail when any application function exceeds the agreed Lizard CCN of 15."""
import argparse
import json
from pathlib import Path
import sys
try:
    import lizard
except ImportError:
    sys.exit('Install development tools: python3 -m pip install -r requirements-quality.txt')
LIMIT = 15
EXTENSIONS = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.m', '.mm'}

def measure(root):
    files = sorted(p for p in root.rglob('*') if p.is_file() and p.suffix in EXTENSIONS)
    functions = []
    for path in files:
        result = lizard.analyze_file(str(path))
        for function in result.function_list:
            functions.append({'file': str(path.relative_to(root)), 'function': function.name,
                              'line': function.start_line, 'ccn': function.cyclomatic_complexity})
    return functions

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--path', type=Path, default=Path(__file__).resolve().parents[1] / 'src')
    parser.add_argument('--json', action='store_true', help='Print machine-readable results')
    options = parser.parse_args()
    functions = measure(options.path.resolve())
    if not functions:
        print('No application functions found; complexity was not checked.', file=sys.stderr)
        return 2
    violations = [f for f in functions if f['ccn'] > LIMIT]
    result = {'limit': LIMIT, 'functions': len(functions),
              'maximum': max(f['ccn'] for f in functions), 'violations': violations,
              'measurements': functions}
    if options.json:
        print(json.dumps(result, indent=2))
    else:
        print(f"{len(functions)} application functions; maximum CCN {result['maximum']}; limit {LIMIT}.")
        for function in violations:
            print(f"{function['file']}:{function['line']} {function['function']}: CCN {function['ccn']}")
    return 1 if violations else 0
if __name__ == '__main__':
    sys.exit(main())
