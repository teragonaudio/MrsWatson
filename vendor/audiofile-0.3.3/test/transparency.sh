#!/bin/sh

for i in aiff aifc next wave
do
	./transparency /tmp/dicko1 /tmp/dicko2 $i ||
		(echo "did not complete test $i." ; exit)
	diff /tmp/dicko1 /tmp/dicko2 ||
		(echo "failed $i format transparency test." ; exit)
done

echo "passed all transparency tests."

rm /tmp/dicko1 /tmp/dicko2
