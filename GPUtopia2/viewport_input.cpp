#include "viewport_input.h"

#include <iostream>

#include "log.h"

#include "compositor.h"
#include "fileOperations.h"
#include "history.h"
#include "layer_manager.h"

namespace input
{
	void handleShortcuts(AppState& app)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clFractal& cf_old = L.cf_old;
		clLayerState& ls = L.kernelState;
		ImGuiIO& io = ImGui::GetIO();
		// Check if Ctrl is held and the key was just pressed (without auto-repeat):
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N, false))
			newCLF(cf, app.core, ls);
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false))
			openCLF(cf, app.core, ls);
		if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_S, false))
			saveCLF(cf);
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_E, false))
			savePNG(app.textureColors, cf.image.size);
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
		{
			undo(cf, cf_old, app.core, ls, L.history, &L.historyIndex);
		}
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false))
		{
			redo(cf, cf_old, app.core, ls, L.history, &L.historyIndex);
		}
	}

	void handleMouse(AppState& app)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		fractalNavigationParameters& nav = app.nav;
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			if (io.KeyCtrl)
			{
				app.imgBlocked = true;
				dragPan(cf, nav, io);
				offsetImageInBox(app.textureColors, app.navPreviewImage, cf, nav);
				refreshTexture(app.textureID, cf.image.size.x, cf.image.size.y, app.navPreviewImage);
			}
			else if (io.KeyShift)
			{
				app.imgBlocked = true;
				dragZoom(cf, nav, io);
				nav.dragStart.x = (int)((io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetMouseDragDelta(0).x)); // / zoom_factor);
				nav.dragStart.y = (int)((io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetMouseDragDelta(0).y)); // / zoom_factor);
				nav.newImgCenter = imgCoordinateCenterAfterZoom(cf, nav);
				nav.newSubplaneCenter = cf.image.image2complex(nav.newImgCenter);
				zoomImageInBox(app.textureColors, app.navPreviewImage, cf, nav);
				refreshTexture(app.textureID, cf.image.size.x, cf.image.size.y, app.navPreviewImage);
			}
			else if (io.KeyAlt)
			{
				app.imgBlocked = true;
				nav.dragStart.x = (int)((io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetMouseDragDelta(0).x)); // / zoom_factor);
				nav.dragStart.y = (int)((io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetMouseDragDelta(0).y)); // / zoom_factor);
				dragRotate(cf, nav, io);
				rotateImageInBox(app.textureColors, app.navPreviewImage, cf, nav);
				refreshTexture(app.textureID, cf.image.size.x, cf.image.size.y, app.navPreviewImage);
			}
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingCenter)
		{
			nav.draggingCenter = false;
			nav.centerOffset = get_complex_offset(nav.dragOffset.x, nav.dragOffset.y, cf);
			cf.image.center.x += nav.centerOffset.x;
			cf.image.center.y += nav.centerOffset.y;
			// Propagate pan to linked layers and trigger re-render
			layers::propagateNavigation(app, L, layers::Center);
			app.textureColors = app.navPreviewImage;
			app.imgBlocked = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingZoom)
		{
			nav.draggingZoom = false;
			// calculate coordinate of pixel in the middle of the displayed image
			cf.image.center = nav.newSubplaneCenter;
			cf.image.zoom *= nav.dragZoomFactor;
			// Propagate zoom to linked layers and trigger re-render
			layers::propagateNavigation(app, L, layers::Center | layers::Zoom);
			logOut(LogLevel::Trace) << "cf.image.center.x " << cf.image.center.x << "\n";
			logOut(LogLevel::Trace) << "cf.image.center.y " << cf.image.center.y << "\n";
			logOut(LogLevel::Trace) << "cf.image.zoom " << cf.image.zoom << "\n";
			app.textureColors = app.navPreviewImage;
			app.imgBlocked = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingRotate)
		{
			nav.draggingRotate = false;
			// calculate coordinate of pixel in the middle of the displayed image
			cf.image.angle += 180. / CL_M_PI * nav.dragAngle;
			// Propagate rotation to linked layers and trigger re-render
			layers::propagateNavigation(app, L, layers::Angle);
			app.textureColors = app.navPreviewImage;
			app.imgBlocked = false;
		}
	}
}
