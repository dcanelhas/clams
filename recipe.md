##Recipe for using CLAMS

##Step 1
make the clams library and apps:
```Shell
mkdir build && cd build && cmake .. && make && cd -
```
##Step 2
make the sdf_tracker library and app:
  cd sdf_tracker && mkdir build && cd build && cmake .. && make && cd -

##Step 3
Use bin/pcl_openni_image to record image sequences (press spacebar to start/stop recording of frames from the camera) 
suppose you record one sequence and save this to sequences/set_01/. 

```Shell
mkdir -p sequences/set_01
```
(NOTE This directory has to be named sequences and contain the individual sequences in its subdirectories)

The important thing to note here is that for best results:
  a. Your sequence should see the *same* surfaces from distances ranging from 0m and up to about 10 m
  b. Your sequence should not challenge your tracking algorithm too much
  c. All pixels in the depth image should be repeatedly exposed to some surface across a range of different distances.

##Step 4 
in your sequences/set_01/ directory you will find a series of images in the following format:
  frame_20140129T160619.901251_depth.pclzf
  frame_20140129T160619.901251_rgb.pclzf
  frame_20140129T160619.901251.xml
For the packaged sdf_tracker-based app you will need to convert these to the freiburg dataset format using

```Shell
mkdir freiburg
```
(NOTE the freiburg/set_01 subdirectory should NOT exist prior to running the following command)

```Shell
./bin/sseq_to_freiburg --src sequences/set_01/ --dst freiburg/set_01
```

##Step 5
Generate a trajectory for this sequence by using the tracker:
```Shell
./sdf_tracker/bin/tracker_clams freiburg/set_01
```

The result is (accuracy considerations aside) a trajectory file (trajectory.txt) in the directory
  freiburg/set_01/trajectory/

##Step 6 
To be used by clams, this trajectory file has to be converted to the clams binary format:

```Shell
  mkdir -p slam_results/set_01/ 
```
(NOTE the directory has to be named "slam_results")

```Shell
./bin/convert_trajectory --sseq sequences/set_01/ --src freiburg/set_01/trajectory/trajectory.txt --dst slam_results/set_01/trajectory
```

(NOTE trajectory has to be named "trajectory" and be in a subdirectory under slam_results)

##Step 7

A "ground truth" map has to be generated against which to calibrate the sensor (given the trajectory). This is done on the premise that the measurements from up-close are good (the mean of several measurents is taken using a voxel grid filter). However, you could generate the calibration_map yourself if you have a nice tsdf reconstruction, for example. 
The parameter --max-range is used to conservatively discard distant measurements. 

```Shell
./bin/generate_map --sseq sequences/set_01/ --traj slam_results/set_01/trajectory --map slam_results/set_01/calibration_map.pcd --max-range 3
```

(NOTE the name of the map file is not optional)

##Step 8 (Optional)
Visualize the map to check if the reconstruction algorithm did its job.

```Shell
./bin/visualize_trajectory --sseq sequences/set_01/ --traj slam_results/set_01/trajectory --map slam_results/set_01/calibration_map.pcd
```
(NOTE press <h> to print help to stdout, <ESC> to close the app)

##Step 9
Generate the calibration. This step requires the directory slam_results to contain one subdirectory for each sequence, each containing the clams format "trajectory" file and a "calibration_map.pcd". There must also be a directory called "sequences" containing stream sequences within subdirectories that match the ones in "slam_results". In other words, you should have the following file structure, at least set_01, set_02 etc is not mandatory but have to be consistently named.

```
clams/
|||
|||-sequences/
||  set_01/<(frames)>
||  set_02/<(frames)>
||
||-slam_results/
|  set_01/<trajectory, calibration_map.pcd>
|  set_02/<trajectory, calibration_map.pcd>
|
|-freiburg/
  ||-set_01/assoc.txt
  |  |||-depth/<(frames)>
  |  ||-rgb/<(frames)>
  |  |-trajectory/trajectory.txt
  |
  |-set_02/assoc.txt
    |||-depth/<(frames)>
    ||-rgb/<(frames)>
    |-trajectory/trajectory.txt
```


Then run

```Shell
./bin/calibrate
```

This might take some time, so you can go and have lunch or read a nice paper.

##Step 10
The calibration results can be visualised within 
  distortion_model-visualization/

##Step 11
You can also run 

```Shell
./bin/inspect
```
(requires an OpenNI camera attached. Press h for help and m to apply the distortion correction )