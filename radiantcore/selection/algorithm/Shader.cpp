#include "Shader.h"

#include "i18n.h"
#include "iselection.h"
#include "iscenegraph.h"
#include "itextstream.h"
#include "iselectiontest.h"
#include "igroupnode.h"
#include "selectionlib.h"
#include "registry/registry.h"
#include "messages/TextureChanged.h"
#include "command/ExecutionFailure.h"
#include "command/ExecutionNotPossible.h"
#include "string/string.h"
#include "brush/FaceInstance.h"
#include "brush/BrushVisit.h"
#include "brush/TextureProjection.h"
#include "patch/PatchNode.h"
#include "selection/algorithm/Primitives.h"
#include "selection/algorithm/Texturing.h"
#include "selection/textool/FaceNode.h"
#include "selection/textool/PatchNode.h"
#include "selection/shaderclipboard/ShaderClipboard.h"
#include "selection/shaderclipboard/ClosestTexturableFinder.h"
#include "scene/Traverse.h"

#include "string/case_conv.h"

namespace selection
{

namespace algorithm
{

void applyShaderToSelectionCmd(const cmd::ArgumentList& args)
{
	if (args.size() != 1 || args[0].getString().empty())
	{
		rMessage() << "SetShaderOnSelection <shadername>" << std::endl;
		return;
	}

	UndoableCommand undo("setShader");
	applyShaderToSelection(args[0].getString());
}

/** greebo: Applies the shader from the clipboard's face to the given <target> face
 */
void applyClipboardFaceToFace(Face& target)
{
	// Get a reference to the source Texturable in the clipboard
	auto& source = ShaderClipboard::Instance().getSource();

	target.applyShaderFromFace(*(source.face));
}

/** greebo: Applies the shader from the clipboard's patch to the given <target> face
 */
void applyClipboardPatchToFace(Face& target)
{
	// Get a reference to the source Texturable in the clipboard
	Texturable& source = ShaderClipboard::Instance().getSource();

	// Copy just the shader name, the rest is default value
	target.setShader(source.patch->getShader());

    // To fix the extremely small scale we get when applying a default TextureProjection
    target.applyDefaultTextureScale();
}

// Function may leak a cmd::ExecutionFailure if the source/target combination doesn't work out
void applyClipboardToTexturable(Texturable& target, bool projected, bool entireBrush)
{
    // Get a reference to the source Texturable in the clipboard
    auto& source = ShaderClipboard::Instance().getSource();

    // Check the basic conditions
    if (target.empty() || source.empty())
    {
        return;
    }

    // Do we have a Face to copy from?
    if (source.isFace())
    {
        if (target.isFace() && entireBrush)
        {
            // Copy Face >> Whole Brush
            for (Brush::const_iterator i = target.brush->begin();
                i != target.brush->end();
                i++)
            {
                applyClipboardFaceToFace(*(*i));
            }
        }
        else if (target.isFace() && !entireBrush)
        {
            // Copy Face >> Face
            applyClipboardFaceToFace(*target.face);
        }
        else if (target.isPatch() && !entireBrush)
        {
            // Copy Face >> Patch

            // Set the shader name first
            target.patch->setShader(source.face->getShader());

            // Either paste the texture projected or naturally
            if (projected)
            {
                target.patch->pasteTextureProjected(source.face);
            }
            else
            {
                target.patch->pasteTextureNatural(source.face);
            }
        }
    }
    else if (source.isPatch())
    {
        // Source holds a patch
        if (target.isFace() && entireBrush)
        {
            // Copy patch >> whole brush
            for (Brush::const_iterator i = target.brush->begin();
                i != target.brush->end();
                i++)
            {
                applyClipboardPatchToFace(*(*i));
            }
        }
        else if (target.isFace() && !entireBrush)
        {
            // Copy patch >> face
            applyClipboardPatchToFace(*target.face);
        }
        else if (target.isPatch() && !entireBrush)
        {
            // Copy patch >> patch
            target.patch->setShader(source.patch->getShader());
            target.patch->pasteTextureNatural(*source.patch);
        }
    }
    else if (source.isShader())
    {
        if (target.isFace() && entireBrush)
        {
            // Copy patch >> whole brush
            for (Brush::const_iterator i = target.brush->begin();
                i != target.brush->end();
                i++)
            {
                (*i)->setShader(source.getShader());
            }
        }
        else if (target.isFace() && !entireBrush)
        {
            target.face->setShader(source.getShader());
        }
        else if (target.isPatch() && !entireBrush)
        {
            target.patch->setShader(source.getShader());
        }
    }
}

void pasteShader(SelectionTest& test, bool projected, bool entireBrush)
{
	// Construct the command string
	std::string command("pasteShader");
	command += (projected ? "Projected" : "Natural");
	command += (entireBrush ? "ToBrush" : "");

	UndoableCommand undo(command);

	// Initialise an empty Texturable structure
	Texturable target;

	// Find a suitable target Texturable
	ClosestTexturableFinder finder(test, target);
	GlobalSceneGraph().root()->traverseChildren(finder);

	if (target.isPatch() && entireBrush)
    {
		throw cmd::ExecutionFailure(
			_("Can't paste shader to entire brush.\nTarget is not a brush."));
	}
	else
    {
		// Pass the call to the algorithm function taking care of all the IFs
		applyClipboardToTexturable(target, projected, entireBrush);
	}

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void pasteTextureCoords(SelectionTest& test)
{
	UndoableCommand undo("pasteTextureCoordinates");

	// Initialise an empty Texturable structure
	Texturable target;

	// Find a suitable target Texturable
	ClosestTexturableFinder finder(test, target);
	GlobalSceneGraph().root()->traverseChildren(finder);

	// Get a reference to the source Texturable in the clipboard
	Texturable& source = ShaderClipboard::Instance().getSource();

	// Check the basic conditions
	if (target.isPatch() && source.isPatch())
    {
		// Check if the dimensions match, emit an error otherwise
		if (target.patch->getWidth() == source.patch->getWidth() &&
			target.patch->getHeight() == source.patch->getHeight())
		{
			target.patch->pasteTextureCoordinates(source.patch);
		}
		else
        {
            throw cmd::ExecutionFailure(
				_("Can't paste Texture Coordinates.\nTarget patch dimensions must match."));
		}
	}
	else
    {
		if (source.isPatch())
        {
			// Nothing to do, this works for patches only
			throw cmd::ExecutionFailure(_("Can't paste Texture Coordinates from patches to faces."));
		}
		else
        {
			// Nothing to do, this works for patches only
			throw cmd::ExecutionFailure(_("Can't paste Texture Coordinates from faces."));
		}
	}

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void pasteShaderName(SelectionTest& test)
{
	// Initialise an empty Texturable structure
	Texturable target;

	// Find a suitable target Texturable
	ClosestTexturableFinder finder(test, target);
	GlobalSceneGraph().root()->traverseChildren(finder);

	if (target.empty())
	{
		return;
	}

	UndoableCommand undo("pasteTextureName");

	// Get a reference to the source Texturable in the clipboard
	Texturable& source = ShaderClipboard::Instance().getSource();

	if (target.isPatch())
	{
		target.patch->setShader(source.getShader());
	}
	else if (target.isFace())
	{
		target.face->setShader(source.getShader());
	}

	SceneChangeNotify();

	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void pickShaderFromSelection(const cmd::ArgumentList& args)
{
	GlobalShaderClipboard().clear();

	const SelectionInfo& info = GlobalSelectionSystem().getSelectionInfo();

	// Check for a single patch
	if (info.totalCount == 1 && info.patchCount == 1)
	{
		try
		{
			Patch& sourcePatch = getLastSelectedPatch();
			ShaderClipboard::Instance().setSource(sourcePatch);
		}
		catch (const InvalidSelectionException&)
		{
			throw cmd::ExecutionNotPossible(_("Can't copy Shader. Couldn't retrieve patch."));
		}
	}
	else if (FaceInstance::Selection().size() == 1)
	{
		try
		{
			auto sourceFace = FaceInstance::Selection().back();
			ShaderClipboard::Instance().setSource(sourceFace->getFace());
		}
		catch (const InvalidSelectionException&)
		{
			throw cmd::ExecutionNotPossible(_("Can't copy Shader. Couldn't retrieve face."));
		}
	}
	else
	{
		// Nothing to do, this works for patches only
		throw cmd::ExecutionNotPossible(_("Can't copy Shader. Please select a single face or patch."));
	}
}

/** greebo: This applies the clipboard to the visited faces/patches.
 */
class ClipboardShaderApplicator
{
	bool _natural;
public:
	ClipboardShaderApplicator(bool natural = false) :
		_natural(natural)
	{}

	void operator()(IPatch& patch)
	{
		try
		{
			Texturable target;

			Patch& targetPatch = dynamic_cast<Patch&>(patch);

			target.patch = &targetPatch;
			target.node = targetPatch.getPatchNode().shared_from_this();

			// Apply the shader (projected, not to the entire brush)
			applyClipboardToTexturable(target, !_natural, false);
		}
		catch (const std::bad_cast&)
		{}
	}

	void operator()(IFace& face)
	{
		try
		{
			Texturable target;

			// Downcast the IFace reference
			Face& targetFace = dynamic_cast<Face&>(face);
			target.face = &targetFace;
			target.node = targetFace.getBrushInternal().getBrushNode().shared_from_this();

			// Apply the shader (projected, not to the entire brush)
			applyClipboardToTexturable(target, !_natural, false);
		}
		catch (const std::bad_cast&)
		{}
	}
};

void pasteShaderToSelection(const cmd::ArgumentList& args)
{
	if (ShaderClipboard::Instance().getSource().empty())
	{
		return;
	}

	// Start a new command
	UndoableCommand command("pasteShaderToSelection");

	ClipboardShaderApplicator applicator;
	GlobalSelectionSystem().foreachFace(applicator);
	GlobalSelectionSystem().foreachPatch(applicator);

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void pasteShaderNaturalToSelection(const cmd::ArgumentList& args)
{
	if (ShaderClipboard::Instance().getSource().empty())
	{
		return;
	}

	// Start a new command
	UndoableCommand command("pasteShaderNaturalToSelection");

	// greebo: Construct a visitor and traverse the selection
	ClipboardShaderApplicator applicator(true); // true == paste natural
	GlobalSelectionSystem().foreachFace(applicator);
	GlobalSelectionSystem().foreachPatch(applicator);

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

TextureProjection getSelectedTextureProjection()
{
	TextureProjection returnValue;

	if (FaceInstance::Selection().size() == 1)
	{
		// Get the last selected face instance from the global
		FaceInstance& faceInstance = *FaceInstance::Selection().back();
		faceInstance.getFace().GetTexdef(returnValue);
	}

	return returnValue;
}

Vector2 getSelectedFaceShaderSize()
{
	Vector2 returnValue(0,0);

	if (FaceInstance::Selection().size() == 1)
	{
		// Get the last selected face instance from the global
		auto& faceInstance = *FaceInstance::Selection().back();

		returnValue[0] = static_cast<Vector2::ElementType>(faceInstance.getFace().getFaceShader().getWidth());
		returnValue[1] = static_cast<Vector2::ElementType>(faceInstance.getFace().getFaceShader().getHeight());
	}

	return returnValue;
}

void fitTexture(const double repeatS, const double repeatT)
{
	UndoableCommand command("fitTexture");

	GlobalSelectionSystem().foreachFace([&] (IFace& face)
    {
        face.fitTexture(static_cast<float>(repeatS), static_cast<float>(repeatT));
    });
	GlobalSelectionSystem().foreachPatch([&] (IPatch& patch)
    {
        patch.fitTexture(static_cast<float>(repeatS), static_cast<float>(repeatT));
    });

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void fitTextureCmd(const cmd::ArgumentList& args)
{
	if (args.size() != 2)
	{
		rWarning() << "Usage: FitTexture <repeatU> <repeatV>" << std::endl;
		return;
	}

	fitTexture(args[0].getDouble(), args[1].getDouble());
}

void flipTexture(int flipAxis)
{
	UndoableCommand undo("flipTexture");

    // Flip every node about its own center point
    GlobalSelectionSystem().foreachFace([&](IFace& face) { TextureFlipper::FlipFace(face, flipAxis); });
	GlobalSelectionSystem().foreachPatch([&] (IPatch& patch) { TextureFlipper::FlipPatch(patch, flipAxis); });
}

void flipTextureS(const cmd::ArgumentList& args)
{
	flipTexture(0);
}

void flipTextureT(const cmd::ArgumentList& args)
{
	flipTexture(1);
}

void naturalTexture(const cmd::ArgumentList& args)
{
	UndoableCommand undo("naturalTexture");

    // Construct the "naturally" scaled Texdef structure
	ShiftScaleRotation shiftScaleRotation;

    float naturalScale = registry::getValue<float>("user/ui/textures/defaultTextureScale");

    shiftScaleRotation.scale[0] = naturalScale;
    shiftScaleRotation.scale[1] = naturalScale;

	// Patches
	GlobalSelectionSystem().foreachPatch(
        [] (IPatch& patch) { patch.scaleTextureNaturally(); }
    );
	GlobalSelectionSystem().foreachFace(
        [&] (IFace& face) { face.setShiftScaleRotation(shiftScaleRotation); }
    );

	SceneChangeNotify();

	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void shiftTexture(const Vector2& shift)
{
	std::string command("shiftTexture: ");
	command += "s=" + string::to_string(shift[0]) + ", t=" + string::to_string(shift[1]);

	UndoableCommand undo(command);

	GlobalSelectionSystem().foreachFace([&] (IFace& face)
    {
        face.shiftTexdefByPixels(static_cast<float>(shift[0]), static_cast<float>(shift[1]));
    });
	GlobalSelectionSystem().foreachPatch([&] (IPatch& patch)
    {
        patch.translateTexture(static_cast<float>(shift[0]), static_cast<float>(shift[1]));
    });

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void scaleTexture(const Vector2& scale)
{
	std::string command("scaleTexture: ");
	command += "sScale=" + string::to_string(scale[0]) + ", tScale=" + string::to_string(scale[1]);

	UndoableCommand undo(command);

	// Prepare the 1.0-based scale value (incoming values are relative to 0)
	Vector2 patchScale = scale + Vector2(1, 1);

    // Flip every node about its own center point
    GlobalSelectionSystem().foreachFace([&](IFace& face) { TextureScaler::ScaleFace(face, patchScale); });
    GlobalSelectionSystem().foreachPatch([&](IPatch& patch) { TextureScaler::ScalePatch(patch, patchScale); });
}

void rotateTexture(const float angle)
{
	std::string command("rotateTexture: ");
	command += "angle=" + string::to_string(angle);

	UndoableCommand undo(command);

	GlobalSelectionSystem().foreachFace([&] (IFace& face) { face.rotateTexdef(angle); });
	GlobalSelectionSystem().foreachPatch([&] (IPatch& patch) { patch.rotateTexture(angle); });
}

void shiftTextureLeft() {
	shiftTexture(Vector2(-registry::getValue<float>("user/ui/textures/surfaceInspector/hShiftStep"), 0.0f));
}

void shiftTextureRight() {
	shiftTexture(Vector2(registry::getValue<float>("user/ui/textures/surfaceInspector/hShiftStep"), 0.0f));
}

void shiftTextureUp() {
	shiftTexture(Vector2(0.0f, registry::getValue<float>("user/ui/textures/surfaceInspector/vShiftStep")));
}

void shiftTextureDown() {
	shiftTexture(Vector2(0.0f, -registry::getValue<float>("user/ui/textures/surfaceInspector/vShiftStep")));
}

void scaleTextureLeft()
{
    // Correct the factor such that clicking the down and up button in the surface inspector
    // brings us back to the original state
    auto step = registry::getValue<float>("user/ui/textures/surfaceInspector/hScaleStep");
	scaleTexture(Vector2(1 / (1 + step) - 1, 0.0f));
}

void scaleTextureRight()
{
	scaleTexture(Vector2(registry::getValue<float>("user/ui/textures/surfaceInspector/hScaleStep"), 0.0f));
}

void scaleTextureUp()
{
	scaleTexture(Vector2(0.0f, registry::getValue<float>("user/ui/textures/surfaceInspector/vScaleStep")));
}

void scaleTextureDown()
{
    // Correct the factor such that clicking the down and up button in the surface inspector
    // brings us back to the original state
    auto step = registry::getValue<float>("user/ui/textures/surfaceInspector/vScaleStep");
	scaleTexture(Vector2(0.0f, 1/(1+step) - 1));
}

void rotateTextureClock() {
	rotateTexture(fabs(registry::getValue<float>("user/ui/textures/surfaceInspector/rotStep")));
}

void rotateTextureCounter() {
	rotateTexture(-fabs(registry::getValue<float>("user/ui/textures/surfaceInspector/rotStep")));
}

void rotateTexture(const cmd::ArgumentList& args) {
	if (args.size() != 1) {
		rMessage() << "Usage: TexRotate [+1|-1]" << std::endl;
		return;
	}

	if (args[0].getInt() > 0) {
		// Clockwise
		rotateTextureClock();
	}
	else {
		// Counter-Clockwise
		rotateTextureCounter();
	}
}

void scaleTexture(const cmd::ArgumentList& args) {
	if (args.size() != 1) {
		rMessage() << "Usage: TexScale 's t'" << std::endl;
		rMessage() << "       TexScale [up|down|left|right]" << std::endl;
		rMessage() << "Example: TexScale '0.05 0' performs"
			<< " a 105% scale in the s direction." << std::endl;
		rMessage() << "Example: TexScale up performs"
			<< " a vertical scale using the step value defined in the Surface Inspector."
			<< std::endl;
		return;
	}

	std::string arg = string::to_lower_copy(args[0].getString());

	if (arg == "up") {
		scaleTextureUp();
	}
	else if (arg == "down") {
		scaleTextureDown();
	}
	if (arg == "left") {
		scaleTextureLeft();
	}
	if (arg == "right") {
		scaleTextureRight();
	}
	else {
		// No special argument, retrieve the Vector2 argument and pass the call
		scaleTexture(args[0].getVector2());
	}
}

void shiftTextureCmd(const cmd::ArgumentList& args) {
	if (args.size() != 1) {
		rMessage() << "Usage: TexShift 's t'" << std::endl
			 << "       TexShift [up|down|left|right]" << std::endl
			 << "[up|down|left|right| takes the step values "
			 << "from the Surface Inspector." << std::endl;
		return;
	}

	std::string arg = string::to_lower_copy(args[0].getString());

	if (arg == "up") {
		shiftTextureUp();
	}
	else if (arg == "down") {
		shiftTextureDown();
	}
	if (arg == "left") {
		shiftTextureLeft();
	}
	if (arg == "right") {
		shiftTextureRight();
	}
	else {
		// No special argument, retrieve the Vector2 argument and pass the call
		shiftTexture(args[0].getVector2());
	}
}

void alignTexture(EAlignType align)
{
	std::string command("alignTexture: ");
	command += "edge=";

	IFace::AlignEdge faceAlignEdge = IFace::AlignEdge::Top;
	IPatch::AlignEdge patchAlignEdge = IPatch::AlignEdge::Top;

	switch (align)
	{
	case ALIGN_TOP:
		command += "top";
		patchAlignEdge = IPatch::AlignEdge::Top;
		faceAlignEdge = IFace::AlignEdge::Top;
		break;
	case ALIGN_BOTTOM:
		command += "bottom";
		patchAlignEdge = IPatch::AlignEdge::Bottom;
		faceAlignEdge = IFace::AlignEdge::Bottom;
		break;
	case ALIGN_LEFT:
		command += "left";
		patchAlignEdge = IPatch::AlignEdge::Left;
		faceAlignEdge = IFace::AlignEdge::Left;
		break;
	case ALIGN_RIGHT:
		command += "right";
		patchAlignEdge = IPatch::AlignEdge::Right;
		faceAlignEdge = IFace::AlignEdge::Right;
		break;
	};

	UndoableCommand undo(command);

	GlobalSelectionSystem().foreachFace([&] (IFace& face) { face.alignTexture(faceAlignEdge); });
	GlobalSelectionSystem().foreachPatch([&] (IPatch& patch) { patch.alignTexture(patchAlignEdge); });

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

void alignTextureCmd(const cmd::ArgumentList& args)
{
    if (args.size() != 1) {
        rMessage() << "Usage: TexAlign [top|bottom|left|right]" << std::endl;
        return;
    }

    const std::string arg = string::to_lower_copy(args[0].getString());

    if (arg == "top")
        alignTexture(ALIGN_TOP);
    else if (arg == "bottom")
        alignTexture(ALIGN_BOTTOM);
    else if (arg == "left")
        alignTexture(ALIGN_LEFT);
    else if (arg == "right")
        alignTexture(ALIGN_RIGHT);
    else
        rMessage() << "Usage: TexAlign [top|bottom|left|right]" << std::endl;
}

void normaliseTexture(const cmd::ArgumentList& args)
{
	UndoableCommand undo("normaliseTexture");

	GlobalSelectionSystem().foreachFace([] (IFace& face) { face.normaliseTexture(); });
	GlobalSelectionSystem().foreachPatch([] (IPatch& patch) { patch.normaliseTexture(); });

	SceneChangeNotify();
	// Update the Texture Tools
	radiant::TextureChangedMessage::Send();
}

class ByShaderSelector :
	public scene::NodeVisitor
{
private:
	std::string _shaderName;

	bool _select;

public:
	ByShaderSelector(const std::string& shaderName, bool select = true) :
		_shaderName(shaderName),
		_select(select)
	{}

	bool pre(const scene::INodePtr& node)
	{
		Brush* brush = Node_getBrush(node);

		if (brush != NULL)
		{
			if (brush->hasShader(_shaderName))
			{
				Node_setSelected(node, _select);
			}

			return false; // don't traverse primitives
		}

		Patch* patch = Node_getPatch(node);

		if (patch != NULL)
		{
			if (patch->getShader() == _shaderName)
			{
				Node_setSelected(node, _select);
			}

			return false; // don't traverse primitives
		}

		return true;
	}
};

void selectItemsByShader(const std::string& shaderName)
{
	ByShaderSelector selector(shaderName, true);
	GlobalSceneGraph().root()->traverseChildren(selector);
}

void deselectItemsByShader(const std::string& shaderName)
{
	ByShaderSelector selector(shaderName, false);
	GlobalSceneGraph().root()->traverseChildren(selector);
}

void selectItemsByShaderCmd(const cmd::ArgumentList& args)
{
	if (args.size() < 1)
	{
		rMessage() << "Usage: SelectItemsByShader <SHADERNAME>" << std::endl;
		return;
	}

	selectItemsByShader(args[0].getString());
}

void deselectItemsByShaderCmd(const cmd::ArgumentList& args)
{
	if (args.size() < 1)
	{
		rMessage() << "Usage: DeselectItemsByShader <SHADERNAME>" << std::endl;
		return;
	}

	deselectItemsByShader(args[0].getString());
}

} // namespace algorithm

} // namespace selection
