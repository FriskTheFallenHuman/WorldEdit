#include "GLTextureManager.h"

#include "imodule.h"
#include "iradiant.h"
#include "itextstream.h"
#include "texturelib.h"
#include "igl.h"
#include "../MapExpression.h"
#include "TextureManipulator.h"
#include "parser/DefTokeniser.h"

namespace
{
    const std::string SHADER_NOT_FOUND = "_missing_texture.png";
}

namespace shaders {

void GLTextureManager::checkBindings()
{
    // Check the TextureMap for unique pointers and release them
    // as they aren't used by anyone else than this class.
    for (TextureMap::iterator i = _textures.begin();
         i != _textures.end();
         /* in-loop increment */)
    {
        // If the std::shared_ptr is unique (i.e. refcount==1), remove it
        if (i->second.use_count() == 1)
        {
            // Be sure to increment the iterator with a postfix ++,
            // so that the iterator is incremented right before deletion
            _textures.erase(i++);
        }
        else
        {
            ++i;
        }
    }
}

TexturePtr GLTextureManager::getBinding(const NamedBindablePtr& bindable,
                                        BindableTexture::Role role)
{
    // Check if we got an empty MapExpression, and return the NOT FOUND texture
    // if so
    if (!bindable)
    {
        return getShaderNotFound();
    }

    // Check if we already have the texture, otherwise construct it
    auto identifier = bindable->getIdentifier();
    auto existing = _textures.find(identifier);
    if (existing != _textures.end())
    {
        // Found, return
        return existing->second;
    }

    // Create and insert texture object, if it is valid
    auto texture = bindable->bindTexture(identifier, role);
    if (texture)
    {
        _textures.emplace(identifier, texture);
        return texture;
    }

    rError() << "[shaders] Unable to load texture: " << identifier << std::endl;
    return getShaderNotFound();
}

TexturePtr GLTextureManager::getBinding(const std::string& fullPath)
{
    // check if the texture has to be loaded
    TextureMap::iterator i = _textures.find(fullPath);

    if (i == _textures.end())
    {
        ImagePtr img = GlobalImageLoader().imageFromFile(fullPath);

        // see if the MapExpression returned a valid image
        if (img != NULL)
        {
            // Constructor returned a valid image, now create the texture object
            TexturePtr texture = img->bindTexture(fullPath);
            _textures[fullPath] = texture;
        }
        else
        {
            rError() << "[shaders] Unable to load texture: "
                                << fullPath << "\n";
            // invalid image produced, return shader not found
            return getShaderNotFound();
        }
    }

    // Cast should succeed since all single image textures will be Texture2D
    return _textures[fullPath];
}

void GLTextureManager::clearCacheForBindable(const NamedBindablePtr& bindable)
{
    if (!bindable) return;

    _textures.erase(bindable->getIdentifier());
}

// Return the shader-not-found texture, loading if necessary
TexturePtr GLTextureManager::getShaderNotFound()
{
    // Construct the texture if necessary
    if (!_shaderNotFound) {
        _shaderNotFound = loadStandardTexture(SHADER_NOT_FOUND);
    }

    // Return the texture
    return _shaderNotFound;
}

TexturePtr GLTextureManager::loadStandardTexture(const std::string& filename)
{
    // Create the texture path
    std::string fullpath = module::GlobalModuleRegistry().getApplicationContext().getBitmapsPath() + filename;

    // load the image with the ImageFileLoader (which can handle .bmp)
    ImagePtr img = GlobalImageLoader().imageFromFile(fullpath);

    if (img)
    {
        // Bind the (processed) texture and get the OpenGL id
        // The getProcessed() call may substitute the passed image by another
        return img->bindTexture(filename);
    }

    rError() << "[shaders] Couldn't load Standard Texture texture: " << filename << "\n";

    return TexturePtr();
}

} // namespace shaders
