NAME = ME1DL2
DESCR = Element de test
DOFSET = DX
NB_NODES = 2
DIM = 1
MODEL = MECHANICS

JACOBIAN[0] = element->nodes[1][0]-element->nodes[0][0]

l=fabs(element->nodes[1][0]-element->nodes[0][0])
K[0] =  E*S/l
K[1] = -E*S/l
K[2] =  E*S/l

M[0] = rho*S*l/3
M[1] = rho*S*l/6
M[2] = rho*S*l/3
