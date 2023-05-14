#include <algorithm>
#include <string>

#include "include/Point.hpp"
#include "include/Slider.hpp"

Slider::Slider(float x, float y, float w, float h) :
	ImageButton("stage-select/slider.png", "stage-select/slider-blue.png", x, y),
	Bar("stage-select/bar.png", x, y, w, h),
	End1("stage-select/end.png", x, y + h / 2, 0, 0, 0.5, 0.5),
	End2("stage-select/end.png", x + w, y + h / 2, 0, 0, 0.5, 0.5) {
	Position.x += w;
	Position.y += h / 2;
	Anchor = Engine::Point(0.5, 0.5);
}
void Slider::Draw() const {
    Bar.Draw();
    End1.Draw();
    End2.Draw();
    ImageButton::Draw();
}
void Slider::SetOnValueChangedCallback(std::function<void(float value)> onValueChangedCallback) {
    OnValueChangedCallback = onValueChangedCallback;
}
void Slider::SetValue(float value) {
	// TODO 1 (4/7): Call 'OnValueChangedCallback' when value changed. Can imitate ImageButton's 'OnClickCallback'. Also, move the slider along the bar, to the corresponding position.
    OnValueChangedCallback(value);
    Position.x = Bar.Position.x + value * Bar.Size.x;
}
void Slider::OnMouseDown(int button, int mx, int my) {
	// TODO 1 (5/7): Set 'Down' to true if mouse is in the slider.
    if(my>=Position.y-Size.y && my<=Position.y+Size.y && mx>Bar.Position.x && mx<Bar.Position.x+Bar.Size.x)
    Down = true;
}
void Slider::OnMouseUp(int button, int mx, int my) {
	// TODO 1 (6/7): Set 'Down' to false.
    Down = false;
}
void Slider::OnMouseMove(int mx, int my) {
	// TODO 1 (7/7): Clamp the coordinates and calculate the value. Call 'SetValue' when you're done
    Engine::ImageButton::OnMouseMove(mx, my);
    if(Down) {
        float value;
        if(mx>Bar.Position.x+Bar.Size.x) {
            value = 1;
        }
        else if(mx<Bar.Position.x) {
            value = 0;
        }
        else {
            value = (mx-Bar.Position.x)/Bar.Size.x;
        }
        SetValue(value);
    }
}
