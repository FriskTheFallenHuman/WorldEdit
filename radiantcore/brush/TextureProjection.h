#pragma once

#include "ibrush.h"
#include "math/Matrix3.h"
#include "math/Matrix4.h"
#include "Winding.h"
#include "TextureMatrix.h"
#include "selection/algorithm/Shader.h"

/**
 * The TextureProjection type contains the TextureMatrix (the six
 * components necessary to produce UV coords from winding vertices)
 * next to a couple of projection algorithms.
 */
class TextureProjection final
{
private:
	TextureMatrix _matrix;

public:
	/**
	 * \brief
	 * Construct a default TextureProjection.
	 *
	 * The projection is initialised with the default texture scale from the
	 * registry.
	 */
	TextureProjection();

	// Copy Constructor
	TextureProjection(const TextureProjection& other);

	// Construct using an existing texture matrix
	TextureProjection(const TextureMatrix& otherMatrix);

	TextureProjection& operator=(const TextureProjection& other);

	void setTransform(const Matrix3& transform);

	// Returns the Shift/Scale/Rotation values scaled to the given image dimensions
	ShiftScaleRotation getShiftScaleRotation(std::size_t width, std::size_t height) const;
	void setFromShiftScaleRotate(const ShiftScaleRotation& ssr, std::size_t width, std::size_t height);

	Matrix3 getMatrix() const;

	// s and t are texture coordinates, not pixels
	void shift(double s, double t);

	// Fits a texture to a brush face
	void fitTexture(std::size_t width, std::size_t height, const Vector3& normal, const Winding& w, float s_repeat, float t_repeat);

	// Aligns this texture to the given edge of the winding
	void alignTexture(IFace::AlignEdge align, const Winding& winding);

	// greebo: Saves the texture definitions into the brush winding points
	void emitTextureCoordinates(Winding& winding, const Vector3& normal, const Matrix4& localToWorld) const;

	// Calculates the UV coords of a single point
	Vector2 getTextureCoordsForVertex(const Vector3& point, const Vector3& normal, const Matrix4& localToWorld) const;

	// greebo: This returns a matrix transforming world vertex coordinates into texture space
	Matrix4 getWorldToTexture(const Vector3& normal, const Matrix4& localToWorld) const;

	// Calculate the texture projection for the desired set of UVs and XYZ
	void calculateFromPoints(const Vector3 points[3], const Vector2 uvs[3], const Vector3& normal);

	// Returns a texture projection using the default scale for the given texture dimensions
	static TextureProjection ConstructDefault(std::size_t width, std::size_t height);

private:
	static TextureMatrix Default();

	Matrix4 getMatrix4() const;
	void setTransformFromMatrix4(const Matrix4& transform);

	// Normalise projection for a given texture width and height.
	void normalise(float width, float height);
};
