#!/bin/bash

astyle --options=./scripts/mrswatson.astyle --recursive "main/*.c" "source/*.c" "source/*.cpp" "source/*.h" "test/*.c" "test/*.h"
