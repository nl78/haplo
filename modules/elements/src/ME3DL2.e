NAME = ME3DL2
DESCR = En cours...
DOFSET = DX:DY:DZ:DRX:DRY:DRZ
NB_NODES = 2
DIM= 3
MODEL = MECHANICS

JACOBIAN[0] = element->nodes[1][0]-element->nodes[0][0]

g = E/(1-nu)
l=sqrt(2) #(X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1)+(Z2-Z1)*(Z2-Z1))
hm=E*S
ht=g*J
h1=E*Iy
h2=E*Iz

K[0]=hm/l
K[1] = 0.0
K[2] = 12*h2/(l*l*l)

K[3] = 0
K[4] = 0
K[5] = 12*h1/(l*l*l)
	
K[6] = 0
K[7] = 0
K[8] = 0
K[9] = ht/l

K[10] = 0
K[11] = 0
K[12] = -6*h1/(l*l)
K[13] = 0
K[14] = 4*h1/l

K[15] = 0
K[16] = 6*h2/(l*l)
K[17] = 0
K[18] = 0
K[19] = 0
K[20] = 4*h2/l

K[21] = -hm/l
K[22] = 0
K[23] = 0
K[24] = 0
K[25] = 0
K[26] = 0
K[27] = hm/l

K[28] = 0
K[29] = -12*h2/(l*l*l)
K[30] = 0
K[31] = 0
K[32] = 0
K[33] = -6*h2/(l*l)
K[34] = 0
K[35] = 12*h2/(l*l*l)

K[36] = 0
K[37] = 0
K[38] = -12*h1/(l*l*l)
K[39] = 0
K[40] = 6*h1/(l*l)
K[41] = 0
K[42] = 0
K[43] = 0
K[44] = 12*h1/(l*l*l)

K[45] = 0
K[46] = 0
K[47] = 0
K[48] = -h1/l
K[49] = 0
K[50] = 0
K[51] = 0
K[52] = 0
K[53] = 0
K[54] = h1/l

K[55] = 0
K[56] = 0
K[57] = -6*h1/(l*l)
K[58] = 0
K[59] = 2*h1/l
K[60] = 0
K[61] = 0
K[62] = 0
K[63] = 6*h1/(l*l)
K[64] = 0
K[65] = 4*h1/l
	
K[66] = 0
K[67] = 6*h2/(l*l)
K[68] = 0
K[69] = 0
K[70] = 0
K[71] = 2*h2/l
K[72] = 0
K[73] = -6*h2/(l*l)
K[74] = 0
K[75] = 0
K[76] = 0
K[77] = 4*h2/l


l=fabs(element->nodes[1][0]-element->nodes[0][0])
M[0] = rho*S*l/3
M[1] = rho*S*l/6
M[2] = rho*S*l/3
