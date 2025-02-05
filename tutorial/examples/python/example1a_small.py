"""small example"""
import sys
sys.path.append("../../.."); sys.path.append("../../../src/python_modules")
import plantbox as pb
import vtk_plot as vp

rs = pb.RootSystem()

# Open plant and root parameter from a file
path = "../../../modelparameter/rootsystem/"
name = "Anagallis_femina_Leitner_2010"
rs.readParameters(path + name + ".xml")

# Initialize
rs.initialize()

# Simulate
rs.simulate(30, True)

for o in rs.getOrgans():
    print(o.param())

# Export final result (as vtp)
rs.write("results/example_1a.vtp")

# Plot, using vtk
vp.plot_roots(rs, "creationTime")
