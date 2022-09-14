
1. Viewport
This is a QOpenGLWidget type class, inheriting base functions for rendering and interfacing with the GPU. Subclasses that require interaction with the GPU depend on the QOpenGLWidget for providing the OpenGL Context.

2.1 Menubar
The file section provides Session and Shader IO. Calling a new session will prompt the user to select an OBJ file. The OBJ parser is custom tailored and requires OBJ be exported from Blender with the following settings:
- Axis Forward: -Z
- Axis Up: Y
- UV Coordinates
- Normals
- Triangulated Mesh

Saved sessions are written using a QTextStream and the resulting files can be inspected with a text editor. The contents are as follows, divided by class.

Mesh ::
- verts     : QVector<vec3>
- idxs      : QVector<GLuint>
- norms     : QVector<vec3>
- texcoords : QVector<vec2>

DataLayer ::
- parameters : QVector<QVector<float>>
- neighbors  : QVector<GLint>

FbSelect ::
- savedSelections : QHash<QString, QVector<uint>>

SessionManager ::
- fragShader : QString
- vertShader : QString
- compShader : QString

Loading a session currently overwrites all data. Shaders can be loaded separately (using the other options in this section) or copy/pasted directly into the Shader Editors.

The Data menu currently has one option: Build Connectivity Array. This can only be performed when a mesh has been loaded. Because this process can take a while on larger meshes, it was designed to be performed optionally rather than on initial load. There is currently no indicator in the GUI as to whether the Datalayer contains adjacency information, but this can be checked using the Shader Watcher.
The algorithm searches for common vertices in the array of facet data, saving the facet ID (FID) for any that shares two vertices with the target facet. The resulting array is indexed based on the FID, and contains the FIDs of the three immediately adjacent facets (or potentially less in some edge cases).

The Help menu contains this document, along with Mouse/Keyboard Bindings, a Shader Cookbook, and About. The Bindings, Cookbook and Readme can all be left open for reference while interacting with the main window.

2.2 Toolbar
This toolbar can be docked to any edge of the main window, or float. The commands are as follows:
- RUN DISPATCH : One-shot trigger for the GPU compute shader.
- GET SHADER WATCH : Prints debug-ssbo data to the Debug Output window.
- GET GL ERROR : Prints accumulated glGetError() messages to Debug Output.
- CLEAR DEBUG: Clears the Debug Output window.
The first three commands have Keyboard Shortcuts that can be found in the bindings reference.

3. Datalayer Toolbox
The Datalayer is a combination of host-side storage and Shader Storage Buffer Objects (SSBOs) that can be manipulated dynamically either by direct assignment in the GUI or by computation in the shaders.
The Toolbox is divided into two main sections: Facet Groups, where selections of facets can be saved and then dynamically loaded, and the Parameter Editing section, which provides direct access to the Datalayer.
Each field in the Parameter Editor has the following:
- radio toggle: only fields with their toggle enabled will send data
- line edit box: this holds the PARAM# name by default. It is not explicitly linked to the SSBO names in the shader, and is provided as a convenience in session building.
- double spin box: the value here will be assigned on Send Data

The basic workflow is that selections are made in the graphics window, and then the respective data can be set per facet in the datalayer using any combination of active parameters. These values can then be subsequently manipulated in the shader editors, and read back with the Shader Watcher.

4. Shader Editors
Exactly what it sounds like. Shows the current shader program and allows for dynamic compilation. Errors will be automatically sent to Debug Output. Note that errors in GLSL compilation do not break the program or cause any crashes (that I've discovered anyway). In the event of an error, simply correct the code and update the shader again.
The vertex and fragment shaders are called regularly by the frontend, but the Compute Shader must be called explicitly using a dispatch call. This is handled using the aforementioned Toolbar Action or shortcut. The potential use case for this shader is truly anything, as it is not directly involved with the rendering pipeline. By utilizing the SSBOs common with the Rendering Shaders, the Compute Shader provides the opportunity to manipulate the shared data using processes of any type. A couple areas worth exploring to start would be dynamic Physically Based Rendering, procedural / deferred rendering, image-processing, and Finite Element Analysis.

5. Debug Output
Our good friend, described in passing above. Displays shader compilation errors, glGetError() dumps, and Shader Watcher feedback.
