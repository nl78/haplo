NAME = ME2DT3
DESCR = Laplacien
DOFSET = DX
NB_NODES = 2
DIM = 2
MODEL=MECHANICS

JACOBIAN[0] = (element->nodes[2][1]-element->nodes[0][1])*(element->nodes[1][0]-element->nodes[0][0]) - (element->nodes[2][0]-element->nodes[0][0])*(element->nodes[1][1]-element->nodes[0][1])

a=element->gauss[0]
b=element->gauss[1]
INTERP_FUNC[0]=(1-a-b)*element->node_values[0] + a*element->node_values[1]+ b*element->node_values[2]


