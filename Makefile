CMAKE = cmake
PRESET = Debug

all: .always
	$(CMAKE) -S . -B ./build \
		-DCMAKE_BUILD_TYPE:STRING=$(PRESET) \
		-DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
		-DBUILD_SHARED_LIBS:BOOL=OFF
	$(CMAKE) --build ./build --config $(PRESET) --parallel 4

clean: .always
	$(CMAKE) -E rm -R -f build

.always:
