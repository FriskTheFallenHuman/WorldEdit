# Set the command name so that WorldEdit recognises this file
__commandName__ = 'ShiftTexturesUpwardsRandomly'
__commandDisplayName__ = 'Shift Textures randomly upwards'

# The actual algorithm called by WorldEdit
# is contained in the execute() function

def execute():
	import random
	import worldedit as dr

	class FaceVisitor(dr.SelectedFaceVisitor) :
		def visitFace(self, face):
			t = random.randint(0, 100) / 100
			face.shiftTexdef(0, t)

	visitor = FaceVisitor()
	GlobalSelectionSystem.foreachSelectedFace(visitor)

	GlobalCameraManager.getActiveView().refresh()

# The variable __executeCommand__ evaluates to true
# when WorldEdit executes this command
if __executeCommand__:
	execute()
