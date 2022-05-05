// Box to house the controller on the side of the kettle


//%kettle_shape();
controller_case();
display_inner_plate();
display_outer_plate();
/* translate([0,4,3]) kettle_wire_clamp();
translate([0,7.5]) wemos_mount_clip(); */
the_ring_of_power();
!drill_template();
module the_ring_of_power(){
    $fn=64;
    FR = 28-1+4;
    difference() {
        union(){
            //cylinder(d=60-11+2,h=1,center=true);
            cylinder(d=68,h=1,center=true);
            /* for(i=[0:3]) rotate([0,0,360*i/3]) hull() for(j=[0:1])
                translate([FR*j,0]) cylinder(d=6,h=1,center=true); */
        }
        cylinder(d=34,h=2,center=true);

        for(i=[0:3]) rotate([0,0,360*i/3]) translate([FR,0]) cylinder(d=2.8,h=2,center=true);

        translate([0,0,0.3]){
            //cylinder(r=17+1,h=1);
            difference() {
                R=21-1;
                W=0.6;
                cylinder(r=R+.5*W,h=1);
                cylinder(r=R-.5*W,h=3,center=true);
            }
            difference() {
                R=21+3;
                W=0.6;
                cylinder(r=R+.5*W,h=1);
                cylinder(r=R-.5*W,h=3,center=true);
            }
            difference() {
                R=25.1+3;
                W=0.7;
                cylinder(r=R+.5*W,h=1);
                cylinder(r=R-.5*W,h=3,center=true);
            }
        }
    }
}

module drill_template(){
    $fn=64;
    difference() {
        union(){
            cylinder(d=28.8,h=4);
            hull() for(i=[0:1]) translate([26*i,0]) cylinder(d=16,h=2);
        }
        translate([22,0]) cylinder(d=2,h=5,center=true);
        translate([26,0]) cylinder(d=2,h=5,center=true);
    }
}

module kettle_shape(){
    cylinder(d1=157.5,d2=132+10*0,h=224,$fn=128);
    translate([0,0,29]) rotate([90,0]) cylinder(d=27,h=80);
    hull() for(i=[-.5:1:.5]) translate([50*i,0,29])
        rotate([90,0]) cylinder(d=11,h=80);
}

module controller_case(){
    $fn=32;
    difference(){
        union(){
            difference(){
                /* hull() for(i=[-.5:1:.5],j=[0:1]) translate([60*i,-58,8+60*j])
                    rotate([90,0]) cylinder(d=16,h=38); */
                hull() for(i=[-.5:1:.5],j=[0:1]) translate([(60+5*0)*i,-58,8+60*j])
                    rotate([90,0]) cylinder(d=16,h=38);
                translate([0,0,-1]) kettle_shape();
                hull() for(i=[-.5:1:.5],j=[0:1]) translate([60*i,-58,8+60*j])
                    rotate([90,0]) cylinder(d=16-4,h=22+16-2+10*0);
            }

            // display screw nub
            for(i=[-.5:1:.5]) translate([0,-96+1,62+17*i])
                rotate([-90,0]) cylinder(d=6,h=2.75);
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(38.5-.2)*i+1,-58-36,62+(13-.2)*j])
                rotate([-90,0]) cylinder(r=3,h=1.75);
            // wemos screw nubs
            color([.4,.4,.8]) hull() for(j=[-.5:1:.5],i=[-.5:1:.5]) translate([-14+38*j,-96+2,26-1+(36+6)*i])
                rotate([-90,0]) cylinder(d=6,h=4+1+2*0);
            // touch sensor nubs
            color([.4,.4,.8]) for(i=[-.5:1:.5]) translate([23,-96+1,40+20*i])
                rotate([-90,0]) cylinder(d=6,h=4-1.9);
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([20.5*i+23,-58-36,40+13*j])
                rotate([-90,0]) cylinder(r=4,h=2-1);
            // kettle connector nubs
            color([.4,.4,.8]) for(i=[-.5:1:.5])hull() for(j=[-.5:1:.5]) translate([23+18*i,-96+1,18+10*j])
                rotate([-90,0]) cylinder(d=6,h=4);
            color([.4,.4,.8]) hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([23-3+(18+10)*i,-96+1,18+10*j])
                rotate([-90,0]) cylinder(d=6,h=2.75);
        }


        color([.6,.2,.9]) translate([0,0,62]){ // OLED
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(26-1)*i,-58,(10-1)*j+.2])
                rotate([90,0]) cylinder(d=1,h=22+16+1);
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(38.5-.2)*i+1,-58,(13-.7)*j])
                rotate([90,0]) cylinder(d=.2,h=22+16-1);
        }

        color([.6,.2,.9]) translate([-14,-3+2,26]){ // WEMOS-32
            /* hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(26-1)*i,-58,(10-1)*j])
                rotate([90,0]) cylinder(d=1,h=22+16+1); */
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(32-2)*i,-58,(40-2)*j])
                rotate([90,0]) cylinder(d=2,h=22+16-4);

            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([(11-1)*i,-58-32+(7-1)*j,-8]) //USB
                rotate([180,0]) cylinder(d=1,h=22+16+1);
        }

        color([.6,.2,.9]) translate([23,0,40]){ // touch sensor
            hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([20.5*i,-58-1.1,13*j])
                rotate([90,0]) cylinder(r=.4,h=22+16-2);
        }

        translate([21,-38-58,40]){
            difference(){ // touch sensor panel button
                hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([10*i,0,10*j])
                    rotate([90,0]) cylinder(r=3.2,h=1,center=true);
                hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([10*i,0,10*j])
                    rotate([90,0]) cylinder(r=2.4,h=2,center=true);
            }
            /* translate([0,0,0]) rotate([90,0]) linear_extrude(height=1,center=true)
                text("Run", size=6, font="Tlwg Typo:style=Bold", halign="center", valign="center",
                    spacing=1.0); */
            hull() for(i=[0:1]) translate([0,0,6*i])
                rotate([90,0]) cylinder(d=1.2,h=1,center=true);

            rotate([90,0]) difference(){
                R = 6;
                lw = 1.2;
                cylinder(r=R,h=1,center=true);
                cylinder(r=R-lw,h=1,center=true);
                hull() for(i=[-.5:1:.5]) translate([0,3*i+R-lw])
                    cylinder(d=3,h=1,center=true);
            }
        }

        fd = 1.8; // fastener diameter
        // display screw nub
        color([.4,.4,.8]) for(i=[-.5:1:.5]) translate([0,-96+1,62+17*i])
            rotate([-90,0]) cylinder(d=fd,h=5);
        // wemos screw nubs
        #color([.4,.4,.8]) for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([-14+38*j,-96+1,26+36*i])
            rotate([-90,0]) cylinder(d=fd,h=5+2*0+5);
        // touch sensor nubs
        color([.4,.4,.8]) for(i=[-.5:1:.5]) translate([23,-96+1,40+20*i])
            rotate([-90,0]) cylinder(d=fd,h=5);
        // touch sensor nubs
        color([.4,.4,.8]) for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([23+18*i,-96+1,18+10*j])
            rotate([-90,0]) cylinder(d=fd,h=5);

        translate([0,-59.25]) rotate([atan(0.5*((132-157.5+8)/132)),0]) cube([200,20,200],center=true);

        // self-tapping M2.3x8mm screws:
        translate([0,-74]) rotate([atan(0.5*((132-157.5+8)/132)),0]) for(i=[-.5:1:.5],j=[0:2]) translate([70*i,0,10+27*j]){
            rotate([0,90,15*i]) cylinder(d=2.35,h=16,center=true);
            rotate([0,180*i,15*i]) translate([0,0,2.75]) cylinder(d=4,h=16); // head
        }
    }
}

module kettle_wire_clamp(){
    $fn=32;
    difference() {
        union(){
            color([.4,.4,.8]) hull() for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([23+18*i,-96+1,15+10*j])
                rotate([-90,0]) cylinder(d=6,h=2);

        }
        for(i=[-.5:1:.5],j=[-.5:1:.5]) translate([23+18*i,-96+1,15+10*j])
            rotate([-90,0]) cylinder(d=2.3,h=4);
        hull() for(i=[-.5:1:.5]) translate([23+11*i,-96+0.5,15]) cylinder(d=2,h=20,center=true);
        hull() for(i=[-.5:1:.5]) translate([23+10*i,-96+0.5,17]) rotate([90,0,0]) cylinder(d=4,h=20,center=true);
    }
}


module wemos_mount_clip(){
    $fn=32;
    difference() {
        union(){
            color([.6,.6,.8]) for(i=[-.5:1:.5]) hull() for(j=[-.5:1:.5]) translate([-14+38*j,-96+1,26+36*i])
                rotate([-90,0]) cylinder(d=6,h=2);
            color([.6,.6,.8]) hull() for(i=[-.5:1:.5]) translate([-14,-96+1,26+36*i])
                rotate([-90,0]) cylinder(d=4,h=2);
        }

        for(j=[-.5:1:.5],i=[-.5:1:.5]) translate([-14+38*j,-96,26+36*i])
            rotate([-90,0]) cylinder(d=2.4,h=4+2*0);
    }
}

module display_inner_plate(){
    $fn=32;
    difference() {
        union(){
            intersection(){ // bit that goes through the hole
                union(){
                    ll = 80;
                    translate([0,0,29]) rotate([90,0]) cylinder(d=27,h=ll);
                    hull() for(i=[-.5:1:.5]) translate([50*i,0,29])
                        rotate([90,0]) cylinder(d=11,h=ll);
                }
                difference() { // kettle surface
                    wt = 1;
                    cylinder(d1=156.5+wt,d2=131+wt,h=224,$fn=128);
                    cylinder(d1=156.5-wt,d2=131-wt,h=224,$fn=128);
                }
            }

            intersection(){ // inner surface
                union(){
                    ll = 80;
                    translate([0,0,29]) rotate([90,0]) cylinder(d=31,h=ll);
                    hull() for(i=[-.5:1:.5],j=[0:1]) translate([50*i+14*i*j,0,29-j*(8+7)])
                        rotate([90,0]) cylinder(d=15-14*j,h=ll);
                }
                difference() { // kettle surface
                    wt = 3;
                    cylinder(d1=156.5,d2=131,h=224,$fn=128);
                    cylinder(d1=156.5-2*wt,d2=131-2*wt,h=224,$fn=128);
                }
            }

        }
        translate([0,0,29]) rotate([90,0]) cylinder(d=27-5,h=80);
        for(i=[-1:2:1],j=[0:1]) translate([(25-10*j)*i,0,29])
            rotate([90,0]) cylinder(d=1.8,h=90);
        //#cylinder(r=100,h=21.5);
    }
}

module display_outer_plate(){
    $fn=32;
    difference() {
        union(){
            intersection(){ // outer surface
                union(){
                    hull() for(i=[-.5:1:.5],j=[0:1]) translate([60*i,-58,8+60*j])
                        rotate([90,0]) cylinder(d=12-.8,h=22+16-2+10*0);
                }
                difference() { // kettle surface
                    wt = 2;
                    cylinder(d1=157.5+2*wt,d2=132+2*wt,h=224,$fn=128);
                    cylinder(d1=157.5,d2=132,h=224,$fn=128);
                }
            }

            intersection(){ // fastener mounting blocks
                intersection(){
                    hull() for(i=[-.5:1:.5]) translate([70*i,-72,2])
                        rotate([atan(0.5*((132-157.5+8)/132)),0]) translate([0,0,-1]) cylinder(d=10,h=73);
                    hull() for(i=[-.5:1:.5],j=[0:1]) translate([60*i,-58,8+60*j])
                        rotate([90,0]) cylinder(d=12-.8,h=22+16-2+10*0);
                }
                /* difference() { // kettle surface
                    wt = 5;
                    cylinder(d1=157.5+2*wt,d2=132+2*wt,h=224,$fn=128);
                    cylinder(d1=157.5,d2=132,h=224,$fn=128);
                } */
                rotate([atan(0.5*((132-157.5+8)/132)),0]) cube([100,152,200],center=true);
            }
        }
        translate([0,0,29]) rotate([90,0]) cylinder(d=27-5,h=90);
        cylinder(d1=157.5,d2=132,h=224,$fn=128);
        //#cylinder(r=100,h=21.5);
        for(i=[-1:2:1],j=[0:1]) translate([(25-10*j)*i,0,29])
            rotate([90,0]) cylinder(d=2.3,h=90);

        hull() for(i=[-.5:1:.5],j=[0:1]) translate([(30+20*j)*i,-58,50+30*j])
            rotate([90,0]) cylinder(d=12-.8,h=22+16-2+10*0);

        // self-tapping M2.3x8mm screws:
        translate([0,-74]) rotate([atan(0.5*((132-157.5+8)/132)),0]) for(i=[-.5:1:.5],j=[0:2])
            translate([70*i,0,10+27*j]) rotate([0,90,15*i]) cylinder(d=1.8,h=16,center=true);
    }
}
