//! \file Graphics.hpp
//! Interface of the Graphics class.

#ifndef GOSU_GRAPHICS_HPP
#define GOSU_GRAPHICS_HPP

#include <Gosu/Fwd.hpp>
#include <Gosu/Color.hpp>
#include <Gosu/GraphicsBase.hpp>
#include <Gosu/TR1.hpp>
#include <memory>

namespace Gosu
{
    //! Returns the width, in pixels, of the user's primary screen.
    unsigned screenWidth();
    
    //! Returns the height, in pixels, of the user's primary screen.
    unsigned screenHeight();
    
    //! Returns the maximum size of an texture that will be allocated
    //! internally by Gosu.
    //! Useful when extending Gosu using OpenGL.
    unsigned const MAX_TEXTURE_SIZE = 1024;
    
    #ifdef GOSU_IS_MAC
    // TODO: Without this gigantic hack, Gosu crashes in the "scale" function,
    // but _only_ when used from Ruby 1.9. It is unclear what might cause this -
    // maybe a compiler bug that tries to use SSE functions with the wrong
    // alignment. Adding __attribute__((aligned(16))) does not help, though.
    struct Transform
    {
        double value[16];
        bool operator==(const Transform &other) { for (int i = 0; i < 16; ++i) if ((*this)[i] != other[i]) return false; return true; }
        const double &operator[](std::size_t idx) const { return value[idx]; }
        double &operator[](std::size_t idx) { return value[idx]; }
    };
    #else
    typedef std::tr1::array<double, 16> Transform;
    #endif
    Transform translate(double x, double y);
    Transform rotate(double angle, double aroundX = 0, double aroundY = 0);
    Transform scale(double factor);
    Transform scale(double factorX, double factorY, double fromX = 0, double fromY = 0);
    
    //! Serves as the target of all drawing and provides primitive drawing
    //! functionality.
    //! Usually created internally by Gosu::Window.
    class Graphics
    {
        struct Impl;
        const std::auto_ptr<Impl> pimpl;

    public:
        Graphics(unsigned physicalWidth, unsigned physicalHeight, bool fullscreen);
        ~Graphics();

        // Undocumented until I have thought about this...
        void setResolution(unsigned virtualWidth, unsigned virtualHeight);
        // End of Undocumented
        
        unsigned width() const;
        unsigned height() const;
        bool fullscreen() const;

        //! Prepares the graphics object for drawing. Nothing must be drawn
        //! without calling begin.
        bool begin(Color clearWithColor = Color::BLACK);
        //! Every call to begin must have a matching call to end.
        void end();
        //! Flushes the Z queue to the screen and starts a new one.
        //! Useful for games that are *very* composite in nature (splitscreen).
        void flush();
        
        //! Finishes all pending Gosu drawing operations and executes
        //! the following OpenGL code in a clean environment.
        void beginGL();
        //! Resets Gosu into its default rendering state.
        void endGL();
        //! (Experimental)
        //! Schedules a custom GL functor to be executed at a certain Z level.
        //! The functor is called in a clean GL context (as given by beginGL/endGL).
        //! Gosu's rendering up to the Z level may not yet have been glFlush()ed.
        //! Note: You may not call any Gosu rendering functions from within the
        //! functor, and you must schedule it from within Window::draw's call tree.
        void scheduleGL(const std::tr1::function<void()>& functor, ZPos z);
        
        //! Enables clipping to a specified rectangle.
        void beginClipping(double x, double y, double width, double height);
        //! Disables clipping.
        void endClipping();
        
        //! Starts recording a macro. Cannot be nested.
        void beginRecording();
        //! Finishes building the macro and returns it as a drawable object.
        //! The width and height affect nothing about the recording process,
        //! the resulting macro will simply return these values when you ask
        //! it.
        //! Most usually, the return value is passed to Image::Image().
        std::auto_ptr<Gosu::ImageData> endRecording(int width, int height);
        
        //! Pushes one transformation onto the transformation stack.
        void pushTransform(const Transform& transform);
        //! Pops one transformation from the transformation stack.
        void popTransform();

        //! Draws a line from one point to another (last pixel exclusive).
        //! Note: OpenGL lines are not reliable at all and may have a missing pixel at the start
        //! or end point. Please only use this for debugging purposes. Otherwise, use a quad or
        //! image to simulate lines, or contribute a better drawLine to Gosu.
        void drawLine(double x1, double y1, Color c1,
            double x2, double y2, Color c2,
            ZPos z, AlphaMode mode = amDefault);

        void drawTriangle(double x1, double y1, Color c1,
            double x2, double y2, Color c2,
            double x3, double y3, Color c3,
            ZPos z, AlphaMode mode = amDefault);

        void drawQuad(double x1, double y1, Color c1,
            double x2, double y2, Color c2,
            double x3, double y3, Color c3,
            double x4, double y4, Color c4,
            ZPos z, AlphaMode mode = amDefault);

        //! Turns a portion of a bitmap into something that can be drawn on
        //! this graphics object.
        std::auto_ptr<ImageData> createImage(const Bitmap& src,
            unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
            unsigned borderFlags);
    };
}

#endif
