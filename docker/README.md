# Build image

`docker build -t "linux_build:build" .`

# Run image

Mount the projects base directory on the image directory `/build`. 

`docker run --rm -v /Users/lfuerste/Development/REXSapi:/build linux_build:build -name rexsapi_linux_build`

The image will execute cmake, build the project and then run the tests.
