"""Contract tests for the local complexity gate; fixtures stay under .scratch."""
import pathlib
import subprocess
import sys
import tempfile
import unittest
ROOT = pathlib.Path(__file__).resolve().parents[1]
class ComplexityGateTests(unittest.TestCase):
    def run_gate(self, source):
        (ROOT / '.scratch').mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=ROOT / '.scratch') as folder:
            if source is not None:
                pathlib.Path(folder, 'fixture.cpp').write_text(source)
            return subprocess.run([sys.executable, str(ROOT / 'scripts/check_complexity.py'), '--path', folder], capture_output=True, text=True)
    def test_simple_function_passes(self):
        self.assertEqual(self.run_gate('int value() { return 1; }').returncode, 0)
    def test_fifteen_ccn_passes(self):
        source = 'int acceptable(int x) {' + 'if(x) ++x;' * 14 + 'return x;}'
        self.assertEqual(self.run_gate(source).returncode, 0)
    def test_sixteen_ccn_fails(self):
        source = 'int risky(int x) {' + 'if(x) ++x;' * 15 + 'return x;}'
        result = self.run_gate(source)
        self.assertEqual(result.returncode, 1)
        self.assertIn('risky', result.stdout)
    def test_empty_scan_is_not_success(self):
        self.assertEqual(self.run_gate(None).returncode, 2)
if __name__ == '__main__':
    unittest.main()
