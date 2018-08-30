# Image Negation Application

Check the PDF file attached to find the particulars of this project.  
(Image Negation means the negative of a grayscale image)

## To build the server

```
cd Server
make
```

## To build the client

```
cd Client
make all_profiled #Enables profiling
make all
```

## To start the server

```
make run_server
```

## To use the client and work with server
```
./userApp <input_image.png> <output_image.png>
```

## Footnotes
- Uses semaphores for synchronization.  
- Uses Shared Memory to share images between processes.
- Multiple clients can use the same server.

### Performance
HD Image  : 1048.2350 ms  
4K Image  : 7113.9190 ms  
330x300px :   46.2860 ms  

### To-Do
- Parallellize the server using OpenMP to handle multiple requests at once.
- Benchmark the performance when multiple clients are using the server at once.

### Important links & References
- [How to clear SharedMem explicitly](https://stackoverflow.com/questions/16630261/creating-my-first-unix-server-client-but-getting-a-shmget-invalid-argument-er)  
- [IPC Semaphore Example](https://gist.github.com/aspyct/3118858)  
- [Shared Memory Example](http://developeriq.in/articles/2012/may/30/interprocess-communicationipc-programs-in-c-in-ubu/)  

Done by [Gokul NC](http://about.me/GokulNC)  
This was an assignment given at [MulticoreWare](http://multicorewareinc.com) to learn Inter-Process Communication in C.
