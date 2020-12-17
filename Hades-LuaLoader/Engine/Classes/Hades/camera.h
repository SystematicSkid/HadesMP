#pragma once

namespace engine::hades
{
	class Camera
	{
	public:
		bool draw_clamps;
		bool use_dynamic_zoom;
		bool use_lazy_tether;
		bool shaking;
		int width;
		int height;
		float min_zoom;
		float soft_clamp;
		float zoom_speed;
		float default_camera_z;
		float zoom;
		float goal_zoom;
		float next_impact_time;
		float move_duration;
		float move_start_time;
		float move_speed;
		float move_acceleration;
		float screen_fx_cooldown;
		float radius_start;
		float radius_end;
		float color_change_speed;
		float impact_texture_fade_speed;
		float prev_move_speed;
		/*
			TODO: Implement vector float classes
		*/
		D3DXVECTOR2 tether_offset;
		D3DXVECTOR2 last_virtual_cursor_location;
		D3DXVECTOR2 prev_location;
		D3DXVECTOR2 prev_goal_location;
		D3DXVECTOR2 prev_cursor_location;
		D3DXVECTOR2 location;
	public:
		static Camera* Instance()
		{
			return static_cast<Camera * (__fastcall*)()>((PVOID)engine::addresses::camera::functions::instance)();
		}

		void Shake(float speed, float distance, float duration, float angle, float easeOutSpeed)
		{
			return static_cast<void(__fastcall*)(Camera*, float, float, float, float, float)>((PVOID)engine::addresses::camera::functions::shake)
				(this, speed, distance, duration, angle, easeOutSpeed);
		}
	};
}