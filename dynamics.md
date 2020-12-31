<h3 id="final">Final project</h3>

#### Triple Pendulum

For our final project, Tim (and other students) was instructed to animate a triple pendulum (three-link chain).
Each link was assumed rigid and each joint was assumed to have no backlash or friction.

Below are the results of three simulations, each solving different differential equations. Left to right:

1. The Newton-Euler equations
2. Differential Algebraic Equations (DAE)
3. The Lagrange equations

<image src="images/dynamics/3xPendulumNE.gif" width="200"></image>
<image src="images/dynamics/3xPendulumDAE.gif" width="200"></image>
<image src="images/dynamics/3xPendulumLE.gif" width="200"></image>

To verify the solution, he compared the trajectories from the three different sets of equations. They were identical, as they should be if the problem were solved correctly.

<image src="images/dynamics/All3LineUp.png" width="500"></image>

#### Four-bar linkage

Students were also instructed to fix the end of the third link to an arbitrary position, forming a four-bar linkage.
He then simulated the un-forced motion of the linkage. The only external force acting on the linkage was gravity.

<image src="images/dynamics/4barLinkageDAE.gif" width="500"></image>

<image src="images/dynamics/4barLinkageDAETrapezoid.gif" width="500"></image>


<h3 id="anim1">First animation</h3>

<iframe src="https://cornell.app.box.com/embed/s/k5lxtdjqi1hvw3jo18to4e1jb8vmivsk?sortColumn=date&view=list" width="500" height="400" frameborder="0" allowfullscreen webkitallowfullscreen msallowfullscreen></iframe>

This is an animation of a baseball hit (launched) at a certain initial velocity. Viscous drag is modeled.
