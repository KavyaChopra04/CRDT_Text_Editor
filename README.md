# SyncScribe

This app demonstrates a collaborative text editor that is built upon a List CRDT inspired by Fugue.

The running instructions are as follows:

1. clone the repository
2. ```cd CRDT_Text_Editor```
3. ```git checkout bundling```
4. ```bash run.sh```

Note that the bash file requires the open command to be installed. In case this command is not installed do this:

1. ```cd ./frontend```
2. manually open client_1.html and client_2.html in a browser

# Functions

Type anything in the text box on any of the clients and the changes should be reflected in real time on both clients. 

Press the disconnect button to disconnect from the other client and do some typing. Then press the connect button to connect again. The changes done during the offline phase should now reflect on both clients.

