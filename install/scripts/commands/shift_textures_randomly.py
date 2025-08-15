# Set the command name so that WorldEdit recognises this file
__commandName__ = 'ShiftTexturesRandomly'
__commandDisplayName__ = 'Shift Textures randomly'

# The actual algorithm called by WorldEdit
# is contained in the execute() function

def execute():
	import random
	import worldedit as dr

	class FaceVisitor(dr.SelectedFaceVisitor) :
		def visitFace(self, face):
			s = random.randint(0, 100) / 100
			t = random.randint(0, 100) / 100
			face.shiftTexdef(s, t)

	visitor = FaceVisitor()
	GlobalSelectionSystem.foreachSelectedFace(visitor)

	GlobalCameraManager.getActiveView().refresh()

# The variable __executeCommand__ evaluates to true
# when WorldEdit executes this command
if __executeCommand__:
	execute()
