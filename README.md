# autoconstruction

[This project has been imported from a Google Project (2013)]

This is a Erasmus Mundus master thesis project in Vision and Robotics (VIBOT). A collaboration work between University of Burgundy, University of Girona, Heriot-Watt University, and Volvo Technology.

The objective of this project is to detect and track the construction workers while they are asphalting streets.

Real-time construction worker detection and tracking (CWDT) improves the safety of construction sites. Moreover, CWDT is an essential prerequisite to automate construction works. In this dissertation, we have proposed a computer vision prototype that exploits the high visibility characteristics of safety jackets. By segmenting the frames in HSV colour space we quickly identified potential constructors. To eliminate false positives, we combined SIFT and LATENT descriptors, trained with an SVM classifier. Lastly, Condensation algorithm was employed to track the constructors.

We evaluated our proposed prototype with video streams of a construction environment for paving roads, i.e. camera in motion. Our primary results evidence great potentials of the proposed prototype with 84.6% accuracy and 94.2% precision. Additionally, our proposed prototype meets its real-time requirements, i.e. computational time of merely 89.3ms per frame with an ordinary laptop.
