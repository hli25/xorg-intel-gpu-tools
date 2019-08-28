#!/bin/sh
#
# Copyright © 2019 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

DOCKERFILE=$1
NAME=$2
REF=${3:-${CI_COMMIT_REF_NAME:-latest}}

REF=$(echo $REF | tr / - )
IMAGENAME=$CI_REGISTRY/$CI_PROJECT_PATH/$NAME
DOCKERFILE_CHECKSUM=$(sha1sum $DOCKERFILE | cut -d ' ' -f1)

REFNAME=$IMAGENAME:$REF
DOCKERNAME=$IMAGENAME:dockerfile-$DOCKERFILE_CHECKSUM
COMMITNAME=$IMAGENAME:commit-$CI_COMMIT_SHA

docker pull $DOCKERNAME
IMAGE_PRESENT=$?

set -e
if [ $IMAGE_PRESENT -eq 0 ] && [ ${FORCE_REBUILD:-0} -eq 0 ] ; then
	echo "Skipping $NAME:$TAG, already built"
	docker tag $DOCKERNAME $NAME
	docker tag $DOCKERNAME $REFNAME
	docker tag $DOCKERNAME $COMMITNAME
else
	echo "Building $NAME:$TAG"
	docker build -t $DOCKERNAME -t $NAME \
		     -t $REFNAME -t $COMMITNAME -f $DOCKERFILE .
	docker push $DOCKERNAME
fi
docker push $REFNAME
docker push $COMMITNAME
if [ $TAG = "master" ]; then
	docker tag $CHECKNAME $IMAGENAME
	docker push $IMAGENAME
fi
