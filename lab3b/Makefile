.SILENT:

# Create a symlink to lab3b
default: lab3b.py
	rm -rf lab3b
	ln -s lab3b.py lab3b
	chmod +x lab3b

# Deletes all makefile-created files
clean:
	rm -rf lab3b lab3b-905129084.tar.gz 

# Build the distribution tarball
dist: 
	tar -czf lab3b-905129084.tar.gz lab3b.py Makefile README
