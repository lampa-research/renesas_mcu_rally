import os
import glob

# get a list of all .c files from all subdirectories
cFiles = glob.glob('**/*.c', recursive=True)
print(cFiles)

# create a subfolder under ../controllers/ for each .c file named after the file
for cFile in cFiles:
    folderName = os.path.splitext(os.path.basename(cFile))[0]
    folderPath = os.path.join('..', 'controllers', folderName)
    os.mkdir(folderPath)
    
    # copy the .c file into the new folder, even if the filename has spaces
    cFilePath = os.path.join(folderPath, os.path.basename(cFile))
    with open(cFile, 'r') as f:
        with open(cFilePath, 'w') as f2:
            f2.write(f.read())

    # open Makefile in read mode
    with open('Makefile', 'r') as f:
        makefile = f.read()
        # replace my_controller with folder name in makefile and copy Makefile to folder
        makefile = makefile.replace('my_controller', folderName)
        with open(os.path.join(folderPath, 'Makefile'), 'w') as f2:
            f2.write(makefile)

# get a list of all .wbo files from all subdirectories
wboFiles = glob.glob('**/*.wbo', recursive=True)

# copy all .wbo files to ../controllers/race/saved_nodes/
for wboFile in wboFiles:
    wboFilePath = os.path.join('..', 'controllers/race/saved_nodes', os.path.basename(wboFile))
    with open(wboFile, 'r') as f:
        with open(wboFilePath, 'w') as f2:
            f2.write(f.read())

# open ../controller/race/competition_cars.txt in write mode
with open(os.path.join('..', 'controllers/race/competition_cars.txt'), 'w') as f:
    # write the number of .wbo files to the file
    f.write(str(len(wboFiles)) + '\n')
    # write the name of each .wbo file to the file 3 times
    for wboFile in wboFiles:
        f.write(os.path.basename(wboFile) + '\n')
        f.write(os.path.basename(wboFile) + '\n')
        f.write(os.path.basename(wboFile) + '\n')
