/*
  CNC GANTRY FRAME – CONCEPTUAL REFERENCE MODEL
  Prepared as a portfolio learning artefact, NOT as a reproduction of the
  applicant's original final-year machine and NOT a fabrication-ready design.

  Dimensions are illustrative. Validate loads, stiffness, fasteners, tolerances,
  motor mounts, bearing supports, guarding, and machine safety before fabrication.
  Open with OpenSCAD; use F6 to render and File > Export > Export as STL.
*/
$fn = 32;

// Illustrative overall dimensions (mm)
frame_x = 600;
frame_y = 450;
frame_z = 120;
rail_w  = 20;
rail_h  = 20;
plate_t = 8;

module beam_x(x,y,z,len,w,h) {
    translate([x,y,z]) cube([len,w,h], center=false);
}
module beam_y(x,y,z,w,len,h) {
    translate([x,y,z]) cube([w,len,h], center=false);
}
module plate(x,y,z,l,w,t) {
    translate([x,y,z]) cube([l,w,t], center=false);
}

// Base perimeter and two cross-members
beam_x(0,0,0,frame_x,rail_w,rail_h);
beam_x(0,frame_y-rail_w,0,frame_x,rail_w,rail_h);
beam_y(0,rail_w,0,rail_w,frame_y-2*rail_w,rail_h);
beam_y(frame_x-rail_w,rail_w,0,rail_w,frame_y-2*rail_w,rail_h);
beam_x(rail_w,frame_y*0.33,0,frame_x-2*rail_w,rail_w,rail_h);
beam_x(rail_w,frame_y*0.66,0,frame_x-2*rail_w,rail_w,rail_h);

// Uprights and top cross-beam (conceptual gantry)
beam_z1 = frame_z;
beam_z2 = frame_z + rail_h;
beam_x(0,0,beam_z1,rail_w,rail_w,beam_z1);
beam_x(frame_x-rail_w,0,beam_z1,rail_w,rail_w,beam_z1);
beam_x(0,frame_y-rail_w,beam_z1,rail_w,rail_w,beam_z1);
beam_x(frame_x-rail_w,frame_y-rail_w,beam_z1,rail_w,rail_w,beam_z1);
beam_x(0,frame_y-rail_w,beam_z2,frame_x,rail_w,rail_h);

// Illustrative gantry carriage plate and toolhead block
plate(frame_x*0.38, frame_y-rail_w-plate_t, beam_z2+rail_h,
      frame_x*0.24, plate_t, plate_t);
translate([frame_x*0.47, frame_y-rail_w-plate_t-35, beam_z2+rail_h-5])
    cube([60,35,55], center=false);

// Work-bed plate (shown raised slightly above base members)
plate(60,55,rail_h+5,frame_x-120,frame_y-110,plate_t);

// Simple mounting-foot pads
for (x=[20,frame_x-50])
  for (y=[20,frame_y-50])
    translate([x,y,-5]) cube([30,30,5],center=false);
