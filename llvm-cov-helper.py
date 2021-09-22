import os
import stat
import sys

from pathlib import Path

profraws = [str(profraw.absolute()) for profraw in Path('.').rglob('*.profraw')]

print(sys.argv)
os.system(f'llvm-profdata merge {" ".join(profraws)} --output coverage.profdata')

executable = stat.S_IEXEC | stat.S_IXUSR
tests = []
for root, dirs, files in os.walk(sys.argv[1]):
	if "CMake" in root: continue
	for filename in files:
		filename = root+"/"+filename
		if os.path.isfile(filename):
			st = os.stat(filename)
			if st.st_mode & executable: tests.append(filename)
regex = "\"(catch)|(elfio/*)|(ngraph)|(magic_enum)|(fmt)|(outcome)|(cereal/*)|(test/*)\""
os.system(f'llvm-cov export --ignore-filename-regex={regex} --instr-profile coverage.profdata --format=lcov {" --object ".join(tests)}> coverage.lcov')
os.system("genhtml -o \"coverage\" coverage.lcov")
