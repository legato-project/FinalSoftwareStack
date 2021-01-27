# Installation of OmpSs@FPGA

Download the repositories under OmpSs-1,

- xdma
- xtasks
- Extrae
- Nanox
- Mercurium
- Automatic Integration Tool (AIT)

and follow these instructions:

**Select an installation directory**

Select your installation directory, where to install the OmpSs@FPGA components:

```

 $ export OMPSS_INSTALL_DIR=\<your-installation-directory\>

```

**Compiling and installing xdma**

Enter the xdma/src/zynq directory, compile and install:

```

 $ cd xdma/src/zynq
 $ export LIBXDMA_DIR=$OMPSS_INSTALL_DIR/libxdma
 $ make 
 $ make PREFIX=$OMPSS_INSTALL_DIR/libxdma install

```

**Compiling and installing xtasks**

Enter the xtasks directory, compile and install:


```
 $ cd xtasks/src/zynq
 $ make
 $ make PREFIX=$OMPSS_INSTALL_DIR/libxtasks install


```
You will observe that, as indicated at the end of the installation, a symbolic link with the name of the actual libxtasks library needs to be created:


```
 $ pushd $OMPSS_INSTALL_DIR/libxtasks/lib; ln -s libxtasks-hwruntime.so libxtasks.so; popd

```


**Compiling and installing Extrae**

Enter the extrae directory, compile and install:
```
 $ cd extrae
 $ ./bootstrap   # needs to be executed only once,
                 # even if you configure Extrae several times
 $ ./configure  --prefix=$OMPSS_INSTALL_DIR/extrae  \
               --without-mpi                        \
               --without-unwind                     \
               --without-dyninst                    \
               --without-papi                       \
               --without-binutils
 $ make
 $ make install
```

**Compiling and installing Nanos++**

Enter the nanox directory, compile and install:


```
 $ cd nanox
 $ ./bootstrap        # needs to be executed only once, even 
                      # if we configure Nanos++ several times 
 $ ./configure  --prefix=$OMPSS_INSTALL_DIR/nanox          \
               --with-extrae=$OMPSS_INSTALL_DIR/extrae     \
               --with-xtasks=$OMPSS_INSTALL_DIR/libxtasks  \
               --disable-allocator --disable-memtracker    \
               --disable-resiliency
 $ make
 $ make install

```

In the output of the Nanos++ configure command, please observe that the following information appears at the end, indicating that we have support for smp and fpga devices: 

```

   Configuration summary of Nanos++
   ================================
    ...
    Configured architectures:  smp fpga
    ...

```

**Compiling and installing the Mercurium compiler**

Enter the mcxx directory, compile and install:

```

 $ cd mcxx
 $ autoreconf --force --install  # needs to be executed only once,
                                 # even if we configure Mercurium
                                 # several times
 $ ./configure  --prefix=$OMPSS_INSTALL_DIR/mcxx      \
               --with-nanox=$OMPSS_INSTALL_DIR/nanox  \
               --enable-ompss                         \
               --enable-nanox-fpga-device             \
               --disable-vectorization
 $ make 
 $ make install

```

** Installing AIT**

Enter the ait directory, and install it:

```

 $ cd ait
 $ ./install.sh $OMPSS_INSTALL_DIR/ait all

```

**Setting up the OmpSs@FPGA environment**

Finally, use a shell script based on this sample to initialize your environment
variables, to be able to access the OmpSs commands:

```

 \<ompss-environment.sh\ sample script>
 export OMPSS_INSTALL_DIR=\<your-installation-directory\>
 export PATH=$OMPSS_INSTALL_DIR/nanox/bin:$PATH
 export PATH=$OMPSS_INSTALL_DIR/mcxx/bin:$PATH
 export PATH=$OMPSS_INSTALL_DIR/ait:$PATH

```

Now, you should be able to compile and run OmpSs@FPGA applications.

For questions/comments, please contact us by e-mail to \<ompss-fpga-support@bsc.es\>.
