# NIWS[^1] Parametric EQ
A Simple Parametric EQ with LFOs to control parameters.

## Purpose

+ To create an open-source EQ.
+ To have in-built LFOs to control the parameters of the filters.
+ To create a GUI identity that feels unique and familiar at the same time.

## Construction

The filters are built using the [Audio EQ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt) recipes. These are IIR biquad filters that are easy to understand in their construction, which is why I've chosen them for this plugin. 

## Current LFO Status

The plugin now has backend support for per-filter LFO modulation on gain-capable bands.

- Each peak filter has its own LFO.
- The low-shelf and high-shelf filters each have their own LFO.
- The current modulation target is filter gain / amplitude.
- LFO parameters currently include enabled state, rate, depth, waveform, and polarity.
- LFO state is saved and restored with the rest of the plugin state.
- No frontend controls have been added for these LFO parameters yet.

## Work in Progress
With no particular order, here is some of the work remaining.

- [ ] Higher Order filters.
- [ ] GUI control for the Q.
- [x] Backend LFO support with variable speed and shape for gain-capable filter bands.
- [x] Per-filter LFO instances for the 4 peak filters and both shelf filters.
- [ ] Frontend controls for the new LFO parameters.
- [ ] Expand LFO modulation to additional filter parameters beyond gain / amplitude.
- [ ] Refine GUI IIR biquad filters
- [x] Basic regression coverage for processor state serialization.
- [ ] Broader unit test coverage.
- [X] Capacity to save presets and reload state.

## Special Mentions
A big part of this project would not have been possible without the big amount of resources available in [Jan Wilczek's (WolfSound)](https://github.com/JanWilczek) github, videos, official website and courses. Modules such as the [JsonSerializer](https://github.com/cuervo-blanco/NIWSParametricEq/blob/main/plugin/include/NIWSParametricEq/JsonSerializer.h) and the [Bypass Transitioner](https://github.com/cuervo-blanco/NIWSParametricEq/blob/main/plugin/include/NIWSParametricEq/BypassTransitioner.h) are inspired directly from WolfSound's official [Juce Development Course](https://www.wolfsoundacademy.com/juce). 

### Footnotes

[^1]:NIWS is a short hand for [Nirioonossian World-Sound](https://gurdjieff.work/ae/neologisms/Nirioonossian.htm). I am deciding to use this name at the moment, although to be honest with you its meaning surpasses my brain. Yes, the universal tone, A.K.A Aum...but with regards to this plugin it may just be a aesthetical choice at the moment, a guiding element. Perhaps on a later version this may be updated and changed.
