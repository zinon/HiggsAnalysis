# Quick instructions:

. Check out the latest svn tag
1. setupATLAS
2. rcSetup Base,2.0.X (where X>=18)
3. Create a local directory xAODs/r5591 in the directory which looks RootCoreBin
3. rc clean
4. rc find_packages
5. rc compile
6. RunMainAnalysis test11 (picks the default job options files and exectutes the code)
7. results are stored in the test11/ output directory 
