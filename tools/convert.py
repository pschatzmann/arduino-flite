
#
# Utility which updates the #include in the .h and .c files to point to an existing valid path
# and and excludes incompatible c files by adding #ifdef Arduino
#
import os
import shutil
from os import listdir
from glob import glob

## An individual ccp or h file 
class FileEntry:
    def __init__(self, root, path):
        self.fullPath = path
        if path.startswith(root):
            self.path = path.replace(root,"", 1)
        else:
            if path.startswith("./"):
                self.path = path[2:]
            else:
                self.path = path

        posType = str(path).rindex(".")
        self.type = path[posType+1:]

        if (str(path).find("/")>=0):
            posName = str(path).rindex("/")
            self.name = path[posName+1:]
        else:
            self.name = path

    def __repr__(self):
        return "{"+self.type+", "+self.path+", "+self.fullPath+"}"+'\n'


## Repository with all cpp and h files 
class Repository:
    dictionary = dict() 
    QUOTES = "\""

    def __init__(self, project, source):
        self.PROJECT = project
        self.SRC = source
        self.__setupWorkingDirectory()
        self.__setupDictionary(self.__findFiles('.','*'))
        print("Repository has been initialized: ", len(self.dictionary))


    def __setupWorkingDirectory(self):
        if os.getcwd().find(self.PROJECT)>0:
            pos = os.getcwd().index(self.PROJECT)
            newPath = os.getcwd()[0:pos+7]
            print("-> new path: ", newPath)
            os.chdir(newPath)
            os.chdir(self.SRC[1:])

        self.root = os.getcwd()    
        print("Directory: ", self.root)


    def __setupDictionary(self, files):
        for file in files:
            entry = FileEntry(self.root, file)
            self.dictionary[entry.name] = entry

    def __findFiles(self, path, match):
        return [y for x in os.walk(path) for y in glob(os.path.join(x[0], match))]

    def __replaceInclude(self, line):
        result = line
        if line.find(self.QUOTES )>-1:
            text = line.replace(self.QUOTES ,"")
            text = text[9:]
            text = text.strip()
            name = self.getName(text)
            if name != '' and self.dictionary.get(name):
                entry = self.getFileEntry(name)
                if entry != None:
                    result = "#include " + self.QUOTES  + entry.path + self.QUOTES + '\n'
                    print(self.getName(text) , '->' , entry.path)
                else:
                    print("Not found: ",self.getName(text))
            else:
                print("Name not defined:", self.getName(text), line)

        return result

    # determines the FileEntry from the file name
    def getFileEntry(self, name):
        return self.dictionary[name]

    # determines the file name for a path
    def getName(self, path):
        entry = FileEntry(self.root, path)
        return entry.name    

    # list of all file names which are in the dictionary
    def listNames(self):
        return self.dictionary.keys()

    # updates the file by replacing the path in the includes
    def updateFile(self, path, isTest=False):
        file = open(path,"rt")
        lines = file.readlines()
        file.close()

        if not isTest:
            file = open(path, "wt")
        for line in lines:
            out = line
            if line.startswith("#include "):
                out = self.__replaceInclude(line)
            if not isTest:
                file.write(out)
        if not isTest:
            file.close()

    def excludeFile(self, path):
        file = open(path,"rt")
        lines = file.readlines()
        file.close()

        if not lines[0].startswith('#ifndef ARDUINO'):
            file = open(path, "wt")
            file.write('#ifndef ARDUINO' + '\n')
            for line in lines:
                file.write(line)
            file.write('#endif')
            file.close()



if __name__ == "__main__":
    # execute only if run as a script
    test = False
    repo = Repository('/flite', '/src')

    # deactivate unsupported implementations
    excludeFiles = ['audio/au_alsa.c', 'audio/au_oss.c','audio/au_palmos.c','audio/au_pulseaudio.c','audio/au_sun.c','audio/au_win.c', 'audio/au_wince.c',
    'utils/cst_file_palmos.c', 'utils/cst_file_wince.c', 'utils/cst_mmap_posix.c', 'utils/cst_mmap_win32.c' ]
    for file in excludeFiles:
        repo.excludeFile(file)

    # convert includes in all files
    for name in repo.listNames():
        entry = repo.getFileEntry(name)
        if entry.type == 'h' or entry.type == 'c':
            print('Processing ', entry.fullPath, '...')
            repo.updateFile(entry.fullPath, test)


