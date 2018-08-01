# Own Race Bias 2009
A cooperation with [Brigitte Daetwyler](http://www.brigittedaetwyler.ch) and [Johnny Nia](http://johnnynia.ch). The installation was used during an exhibition at [Art School Lucerne](https://www.hslu.ch/en/lucerne-school-of-art-and-design/).

## Idea
[Own race bias is a term used in theories of cognition to describe mistakes when interpreting faces of alien races. The appearance of members from a foreign cultural background is significantly less differentiatedly perceived than the one of members from a familiar cultural background.](http://www.brigittedaetwyler.ch/mixed_media/own_race_bias)

## Execution
Photographs taken by an Indian street photographer with a camera obscura were used as basis for an algorithmic analysis of the pictures and faces. One approach focused on the image data, the other on recognising and comparing the face in the photograph.

### Sorting photographs by grayscale in a binary search tree
The wall picture shown at the exhibition consists of the original photographs. The grayscale value of the photographs was used to arrange the wall picture based on a binary search tree.

### Detecting, sorting and morphing faces with computer vision
An [OpenCV](https://opencv.org) [script](sort/main.cpp) was used to detect the face and eyes of each person in the image. The [generated data](sort/bin/analysed_images.csv) was then used to sort the pictures according to the detected position of the eyes in the images. The [generated list](sort/bin/sorted_pics.txt) was then used to [morph](morph/renderer.sh) consecutive images.

For calculating the intermediate frames of the morphing sequence a small [command line tool](https://code.google.com/archive/p/optflow/) was used. It included a morphing algorithm based on [Proesmans' optical flow algorithm](https://link.springer.com/chapter/10.1007/BFb0028362). This algorithm worked better with the low image quality than [OpenCV based morphing](morph/main.cpp).

The calculated frames where then used to generate a [video file](movie/outputfile.mkv) with [ffmpeg](http://ffmpeg.org).

It was a conscious decision to not interfere with the mechanical sorting and merging of the faces in the photographs. False negatives and positives were an accepted outcome. Photographs, which did not trigger a detection of a face and eyes where mechanically sorted out. Photographs were face and eyes were detected outside of their actual position were mechanically kept in the process and lead to morphing errors in the output.
