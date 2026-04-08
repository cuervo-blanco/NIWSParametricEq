# NIWS[^1] Parametric EQ
A Simple Parametric EQ with LFOs to control parameters.

## Purpose

+ To create an open-source EQ.
+ To have in-built LFOs to control the parameters of the filters.
+ To create a GUI identity that feels unique and familiar at the same time.

## Construction

The filters are built using the [Audio EQ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt) recipes. These are IIR biquad filters that are easy to understand in their construction, which is why I've chosen them for this plugin. 

## Current Interface Highlights

- The spectrum analyzer is now drawn as a discrete stem plot, so visible FFT bins appear as vertical sticks with circular markers rather than as a continuous trace.
- Clicking a filter handle opens a filter inspector panel with the selected band's full settings.
- The inspector now includes a close button so the panel can be dismissed without selecting another band.
- The inspector exposes frequency, Q, slope, bypass, gain, and available LFO controls for the selected band.
- Main editor `Post` and `Bypass` toggle buttons are now always available so the analyzer source and the whole-EQ bypass state can be changed quickly.
- The macOS standalone build now includes microphone permission metadata so it can request audio-input access properly on first launch.

## Current LFO Status

The plugin now has backend support for per-filter LFO modulation on gain-capable bands.

- Each peak filter has its own LFO.
- The low-shelf and high-shelf filters each have their own LFO.
- The current modulation target is filter gain / amplitude.
- LFO parameters currently include enabled state, rate, depth, waveform, and polarity.
- LFO state is saved and restored with the rest of the plugin state.
- Frontend controls for the current LFO parameters are available from the filter inspector when a gain-capable band is selected.

## Using The Current UI

- Click a band handle to open that filter's inspector panel.
- Use the inspector to adjust detailed controls that are not directly draggable from the graph.
- Use `Post` to switch the analyzer between pre-EQ and post-EQ monitoring.
- Use `Bypass` to compare processed and unprocessed sound quickly.
- In the standalone app on macOS, grant microphone access when prompted so live input can reach the analyzer and processing chain.

## Work in Progress
With no particular order, here is some of the work remaining.

- [ ] Higher Order filters.
- [x] GUI control for the Q through the filter inspector.
- [x] Backend LFO support with variable speed and shape for gain-capable filter bands.
- [x] Per-filter LFO instances for the 4 peak filters and both shelf filters.
- [x] Frontend controls for the current LFO parameters through the filter inspector.
- [ ] Expand LFO modulation to additional filter parameters beyond gain / amplitude.
- [ ] Refine GUI IIR biquad filters
- [ ] Continue polishing the main editor layout and utility controls.
- [x] Basic regression coverage for processor state serialization.
- [ ] Broader unit test coverage.
- [X] Capacity to save presets and reload state.

## Special Mentions
A big part of this project would not have been possible without the big amount of resources available in [Jan Wilczek's (WolfSound)](https://github.com/JanWilczek) github, videos, official website and courses. Modules such as the [JsonSerializer](https://github.com/cuervo-blanco/NIWSParametricEq/blob/main/plugin/include/NIWSParametricEq/JsonSerializer.h) and the [Bypass Transitioner](https://github.com/cuervo-blanco/NIWSParametricEq/blob/main/plugin/include/NIWSParametricEq/BypassTransitioner.h) are inspired directly from WolfSound's official [Juce Development Course](https://www.wolfsoundacademy.com/juce). 

### Footnotes

[^1]:NIWS is a short hand for [Nirioonossian World-Sound](https://gurdjieff.work/ae/neologisms/Nirioonossian.htm). I am deciding to use this name at the moment, although to be honest with you its meaning surpasses my brain. Yes, the universal tone, A.K.A Aum...but with regards to this plugin it may just be a aesthetical choice at the moment, a guiding element. Perhaps on a later version this may be updated and changed.
