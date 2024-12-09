#pragma once

struct FadeComponent
{
	bool FadeIn = true;
	bool IsFadeInDone = false;
	bool IsFadeOutDone = false;

	void SetFadeIn(bool enabled)
	{
		FadeIn = enabled;
	}

	void SetFadeOut(bool enabled)
	{
		FadeIn = !enabled;
	}

	FadeComponent() = default;
	FadeComponent(const FadeComponent&) = default;
};