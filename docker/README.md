
# Build Image

`docker build -t "linux_build:build" .`

# Run Image

Mount the projects base directory on the image directory `/build`. 

`docker run --rm -v /Users/lfuerste/Development/REXSapi:/build linux_build:build -name rexsapi_linux_build`

The image will execute cmake, build the project and then run the tests.

# Run Image for a Coverage Build

`docker run --rm -e COVERAGE=On -v /Users/lfuerste/Development/REXSapi:/build linux_build:build -name rexsapi_linux_build`

The coverage result html files can be found under `/Users/lfuerste/Development/REXSapi/linux-build/html`
