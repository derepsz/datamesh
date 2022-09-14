---
## 22-0817
---
### Last Sesh
- reset connectivity array
- save param convenience names
- hdf5 file format

---
## 22-0803/0810
---

### Minutes
- "New Session" over load model
- hash symbols in session files for comments
- add double click for list selection
- check if DebugContext can be turned off...
  - realtime
  - on model load
  - splash screen dialog option
  - create multiple contexts at start and switch 
  - vertical orientation for params on LHS
- Sanford - Robertson model
  - http://web.gps.caltech.edu/~vijay/Papers/BRDF/shell-04.pdf
  - https://www.spectral.com/wp-content/uploads/2017/04/Bidirectional_Reflectance_Distribution-1.pdf


---
## 22-0721
---

### Minutes
- Last day Friday 19th
  - final present that week (excpet Thurs)

- priorities
  - easy hand off to colin's team
  - load / save / etc independently
  - investigate fbo select issue
    - monitor scaling issue
  - connectivity list
  - documentation / knowledge transfer
  - BxDF class, btwn learnopengl and pbrt

### learnopengl PBR notes
https://learnopengl.com/PBR/Theory
- 3 conditions to be physically based
  1. Be based on the microfacet surface model.
  2. Be energy conserving.
  3. Use a physically based BRDF.
- tutorial uses *metallic workflow* (Disney / Epic)

- **microfacets**
  - surfaces comprised of many microscopic facets
  - roughness determined by relative alignment
  - roughness increases scatter
  - specularity affected by roughness parameter
    - ratio of microfacets aligned with halfway vector
    - see blinn-phong

![image](https://user-images.githubusercontent.com/56654899/181571543-23c75f64-d270-4123-9bd6-508f8f651184.png)

- **energy conservation**
  - reflection + refraction = specular + diffuse
  - ray energy lost in subsurface scatter
  - metallic surfaces (vs dielectric)
    - all refracted light directly absorbed - no scatter
    - all specular, no diffuse
  - reflected / refracted light mutually exclusive
    - energy preservation
    - specular component (kS) / reflection calculated first
    - kD = 1.0 - kS; (refraction/diffuse fraction)
    
![surface_reaction](https://user-images.githubusercontent.com/56654899/181574473-3044b976-9a53-4571-8195-1a1c69e4d234.png)

- **the reflectance equation**  
![image](https://user-images.githubusercontent.com/56654899/181576813-94a8aa5f-4620-492f-96df-4556da96c5ba.png)
  - solid angle : range of incoming/outgoing light scatter
  - radiant intensity : flux per solid angle
    - strength of light source over projected area onto unit sphere
    - I = d(Phi) / d(omega)
  - radiance equation
  </br>![image](https://user-images.githubusercontent.com/56654899/181579531-0c4f504f-1b60-412b-8ec9-fa160e272184.png)
    
  - irradiance : sum of all incoming light within hemisphere Omega
    - Omega centered at point p, aligned with surface normal n
  - "The reflectance equation sums up the radiance of all incoming light directions ωi over the hemisphere Ω scaled by fr that hit point p and returns the sum of reflected light Lo in the viewer's direction."

- **BRDF**
  - variables
    - wi : incoming light direction
    - wo : outgoing light direction
    - n  : surface normal
    - a  : microsurface roughness
  - Cook-Torrance Model: fr = kd * f_lambert + ks * f_CookTorrance
    - diffuse surface texture : f_lambert = c / pi, where c = albedo
    - f_CookTorrance = (DFG) / (4(wo*n)(wi*n))
      - D : normal distribution function
      - F : Fresnel equation
      - G : Geometry function
      - denominator : normalization factor

![image](https://user-images.githubusercontent.com/56654899/181597637-675ca6d6-0b6e-46a7-a440-b675c85374c4.png)


  

---
## 22-0721
---
### Minutes
- Get error from GL Compiler when it breaks
- Hot swap of datalayer
- check out nvidia nsight
  - GPU compute... symmetric multiprocessors -> CUDA cores
  - each CUDA has local register space
- independent shaders for different BRDFs, switchable
- check BRDF class in pbr text
- phong shader with adjustable params as test
- unique datalayer format per brdfs


---
## 22-0711
---
### Minutes
- Scale model on load
- Selection issue - screen scale
  - update screen scaling parameter
- error handling exceptions
  - OpenGL Debugging module in Qt
- "scenegraph" / scene management libraries
- microfacet model
- area lights vs point models
- editable brdf model
- revisit volume rendering assignment from scivis
- taking parameters in to brdf function in shader
- ray-triangle intersction models
- review shader wrapper

---
## 22-0705
---
### Updates (App)
- Colorscale w/ adjustable min/max
- Simple shader
- Select model on open
- Coordinate axis (kinda)
- UI Improvements
  - improved cam control
  - set temp with right-click
  
### Updates (General)
- scratch Finite Element features
- scratch interactivity (arduino)
- maybe vision system
- definitely PBRT
- definitely deferred rendering
- probably concurrent / multithreaded operation

### Minutes
- Realtime rendering book for C++ data handling
  - vol 4
- vulcan - many threads between cpu/gpu 

---

## Needs
- OBJ import dialogue
  - Mesh/Model-Class w/ independent VBOs
  - Improved assimp parsing
- Better camera control
- Painting
- Min / max temp spec
- Selectable colormap

## Wants
- Release build
- Proper window layout
  - File menus
  - status bar
  - Viewport resizing
- Scene-building tools
  - Multiple model handling
  - Manipulate models
- Nearest-neighbor data
  - For FEM heat xfr..?
- Mesh tools
  - Analysis
  - Vertex movement 
- Lights

---
---
## 22-0601
---
### Updates
- Multiple Select w/ Rendered Rect
- Assign temp to selected
- SSBOs for variable length data
  - temperatures
  - selected faces
- save / load temps as binary

### Minutes
- prioritize running release build over menus etc
- painter lower priority vs colormap legend 
- look into progressive rendering while solver runs
- BRDF 
  - each facet has it's own brdf
  - ability to visual and manipulate
  - has to be ray-traced
  - ref pbr text

---
## 22-0517
---
- System architecture v0

---
## 22-0510
---
- Write color (temp) to selected facet
- Mouse control

---
## 22-0505
---
- 3D select: Framebuffer + Tex 
- Highlight selected
- Track selected ID

---
## 22-0503
---
- Assimp loader w/ face IDs
- UI Info (Tri count)

---
## 22-0426
---
- Loaded model w/ custom OBJ parser
- Assimp working-ish
- Encapsulation / modularity - 'TVCore' 
- Keyboard / mouse events

### Picking / Selection Methods
- Framebuffer
- Ray-tracing / intersection
- Physics lib -> collision
- Acceleration structures

---
## 22-0415
---
- Environment - Qt Creator, ASSIMP
- Qt Widgets ~~vs Qt Quick~~
- Qt OpenGL Classes
- Hello Qt OpenGL
