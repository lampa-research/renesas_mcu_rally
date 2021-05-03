# Building 3D tracks

Custom tracks can be built by using the provided assets.

![track_assets_3d](img/track_assets_3d.png)
Example use of 3D track assets.

## Good practice

It is recommended to use a Group node (found under Base nodes) as the root node when building tracks. To add individual assets, Add Node, then navigate to PROTO nodes (Current Project) -> track_parts_3d.

To save a track for later use, check the [PROTO manuals](https://cyberbotics.com/doc/reference/proto) in the WeBots documentation. Saving custom PROTO nodes involves opening the world definition in a text editor, copying the part with the track Group node to a .proto file, and placing the proto file into the protos folder of the project.
