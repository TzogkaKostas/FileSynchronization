# FileSynchronization

This application is a simplified version of the dropbox. Different processes synchronize a set of files. 
The processes work the same way and their purpose is to communicate and synchronize with each other process, 
so that each process has a copy of the files of all other processes.


# Execution
./mirror_client -n id -c common_dir -i input_dir -m mirror_dir -b buffer_size -l log_file

- id: is a number that represents an identifier for that client.

- common_dir: is a directory that will be used to communicate with each other
mirror_clients.

- input_dir: is a directory containing the files that the client will distribute.

- b: is the size of the buffer to read over the pipes.

- mirror_dir: is a directory containing files from all clients involved
in the mirroring application.

- log_file: all messages during the execution are written in this file. 
