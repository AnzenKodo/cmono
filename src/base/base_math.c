// Scalar Math Ops
//=============================================================================

internal float remainder_f32(float x, float y)
{
	return x - (round_f32(x/y)*y);
}
internal float fmod_f32(float x, float y)
{
	float result;
	y = Abs(y);
	result = remainder_f32(Abs(x), y);
	if (Sign(result)) result += y;
	return copysign_f32(result, x);
}

internal double remainder_f64(double x, double y)
{
	return x - (round_f64(x/y)*y);
}
internal double fmod_f64(double x, double y)
{
	double result;
	y = Abs(y);
	result = remainder_f64(Abs(x), y);
	if (Sign(result)) result += y;
	return copysign_f64(result, x);
}

// Exponential functions ======================================================

internal float exp_f32(float a)
{
    union { float f; int i; } u, v;
    u.i = (int)(6051102 * a + 1056478197);
    v.i = (int)(1056478197 - 6051102 * a);
    return u.f / v.f;
}
internal float exp2_f32(float x)
{
    return exp_f32(LOG_TWO_F32 * x);
}
internal float log_f32(float a)
{
    union { float f; int i; } u = {a};
    return (u.i - 1064866805) * 8.262958405176314e-8f; /* 1 / 12102203.0; */
}
internal float log2_f32(float x)
{
    return log_f32(x) / LOG_TWO_F32;
}

internal double exp_f64(double a)
{
    union { double f; int64_t i; } u, v;
    int64_t k = (int64_t)(3248660424278399LL * a);
    int64_t bias = 4602417669361729536LL;
    u.i = k + bias;
    v.i = bias - k;
    return u.f / v.f;
}
internal double exp2_f64(double x)
{
    return exp_f64(LOG_TWO_F64 * x);
}
internal double log_f64(double a)
{
    union { double f; int64_t i; } u = {a};
    return (double)(u.i - 4606921279571168416LL) * 1.539095918623324e-16;
}
internal double log2_f64(double x)
{
    return log_f64(x) / LOG_TWO_F64;
}

// Power functions ============================================================

internal float sqrt_f32(float number)
{
    // Error handling for negative numbers
    if (number < 0)
    {
        return -1.0;
    }
    if (number == 0 || number == 1)
    {
        return number;
    }
    float guess = number / 2.0;
    float tolerance = 0.00001;
    while (Abs((guess * guess) - number) > tolerance)
    {
        guess = 0.5 * (guess + number / guess);
    }

    return guess;
}
internal float rsqrt_f32(float a)
{
	union {
		int i;
		float f;
	} t;
	float x2;
	float const three_halfs = 1.5f;
	x2 = a * 0.5f;
	t.f = a;
	t.i = 0x5f375a86 - (t.i >> 1);                /* What the fuck? */
	t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 1st iteration */
	t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 2nd iteration, this can be removed */
	return t.f;
}
internal float pow_f32(float a, float b)
{
    int32_t flipped = 0, e;
    float f, r = 1.0f;
    if (b < 0) {
        flipped = 1;
        b = -b;
    }
    e = (int32_t)b;
    f = exp_f32(b - e);
    while (e) {
        if (e & 1) r *= a;
        a *= a;
        e >>= 1;
    }
    r *= f;
    return flipped ? 1.0f/r : r;
}

internal double sqrt_f64(double number)
{
    // Error handling for negative numbers
    if (number < 0)
    {
        return -1.0;
    }
    if (number == 0 || number == 1)
    {
        return number;
    }

    double guess = number / 2.0;
    double next_guess = 0.0;
    double tolerance = 1e-12;

    while (1)
    {
        next_guess = 0.5 * (guess + number / guess);
        if (Abs(guess - next_guess) < tolerance) {
            break;
        }

        guess = next_guess;
    }

    return next_guess;
}
internal double rsqrt_f64(double a)
{
    union {
        int64_t i;
        double f;
    } t;
    double x2;
    double const three_halfs = 1.5;
    x2 = a * 0.5;
    t.f = a;
    // The Magic Number for 64-bit Double Precision
    t.i = 0x5fe6eb50c7b537a9LL - (t.i >> 1);
    t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 1st iteration */
    t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 2nd iteration */
    return t.f;
}
internal double pow_f64(double a, double b)
{
    int32_t flipped = 0;
    int64_t e;
    double f, r = 1.0;
    if (b < 0) {
        flipped = 1;
        b = -b;
    }
    e = (int64_t)b;
    f = exp_f64(b - e);
    while (e) {
        if (e & 1) r *= a;
        a *= a;
        e >>= 1;
    }
    r *= f;
    return flipped ? 1.0 / r : r;
}

// Trigonometric functions ====================================================


internal float sin_f32(float a)
{
    static float const a0 = +1.91059300966915117e-31f;
    static float const a1 = +1.00086760103908896f;
    static float const a2 = -1.21276126894734565e-2f;
    static float const a3 = -1.38078780785773762e-1f;
    static float const a4 = -2.67353392911981221e-2f;
    static float const a5 = +2.08026600266304389e-2f;
    static float const a6 = -3.03996055049204407e-3f;
    static float const a7 = +1.38235642404333740e-4f;
    return a0 + a*(a1 + a*(a2 + a*(a3 + a*(a4 + a*(a5 + a*(a6 + a*a7))))));
}
internal float cos_f32(float a)
{
    static float const a0 = +1.00238601909309722f;
    static float const a1 = -3.81919947353040024e-2f;
    static float const a2 = -3.94382342128062756e-1f;
    static float const a3 = -1.18134036025221444e-1f;
    static float const a4 = +1.07123798512170878e-1f;
    static float const a5 = -1.86637164165180873e-2f;
    static float const a6 = +9.90140908664079833e-4f;
    static float const a7 = -5.23022132118824778e-14f;
    return a0 + a*(a1 + a*(a2 + a*(a3 + a*(a4 + a*(a5 + a*(a6 + a*a7))))));
}
internal float tan_f32(float radians)
{
    float rr = radians*radians;
    float a = 9.5168091e-03f;
    a *= rr;
    a += 2.900525e-03f;
    a *= rr;
    a += 2.45650893e-02f;
    a *= rr;
    a += 5.33740603e-02f;
    a *= rr;
    a += 1.333923995e-01f;
    a *= rr;
    a += 3.333314036e-01f;
    a *= rr;
    a += 1.0f;
    a *= radians;
    return a;
}
internal float asin_f32(float a)
{
    return atan2_f32(a, sqrt_f32((1.0f + a) * (1.0f - a)));
}
internal float acos_f32(float a)
{
    return atan2_f64(sqrt_f32((1.0f + a) * (1.0 - a)), a);
}
internal float atan_f32(float a)
{
    float u  = a*a;
    float u2 = u*u;
    float u3 = u2*u;
    float u4 = u3*u;
    float f  = 1.0f+0.33288950512027f*u-0.08467922817644f*u2+0.03252232640125f*u3-0.00749305860992f*u4;
    return a/f;
}
internal float atan2_f32(float y, float x)
{
    float result;
    if (Abs(x) > Abs(y))
    {
        float a = atan_f32(y/x);
        if (x > 0.0f)
        {
            result = a;
        }
        else
        {
            result = y > 0.0f ? a+TAU_OVER_2_F32:a-TAU_OVER_2_F32;
        }
    }
    else
    {
        float a = atan_f32(x/y);
        if (x > 0.0f)
        {
            result = y > 0.0f ? TAU_OVER_4_F32-a:-TAU_OVER_4_F32-a;
        }
        else
        {
            result = y > 0.0f ? TAU_OVER_4_F32+a:-TAU_OVER_4_F32+a;
        }
    }
    return result;
}

internal double sin_f64(double a)
{
    static double const a0 = +1.91059300966915117e-31f;
    static double const a1 = +1.00086760103908896f;
    static double const a2 = -1.21276126894734565e-2f;
    static double const a3 = -1.38078780785773762e-1f;
    static double const a4 = -2.67353392911981221e-2f;
    static double const a5 = +2.08026600266304389e-2f;
    static double const a6 = -3.03996055049204407e-3f;
    static double const a7 = +1.38235642404333740e-4f;
    return a0 + a*(a1 + a*(a2 + a*(a3 + a*(a4 + a*(a5 + a*(a6 + a*a7))))));
}
internal double cos_f64(double a)
{
    static double const a0 = +1.00238601909309722f;
    static double const a1 = -3.81919947353040024e-2f;
    static double const a2 = -3.94382342128062756e-1f;
    static double const a3 = -1.18134036025221444e-1f;
    static double const a4 = +1.07123798512170878e-1f;
    static double const a5 = -1.86637164165180873e-2f;
    static double const a6 = +9.90140908664079833e-4f;
    static double const a7 = -5.23022132118824778e-14f;
    return a0 + a*(a1 + a*(a2 + a*(a3 + a*(a4 + a*(a5 + a*(a6 + a*a7))))));
}
internal double tan_f64(double radians)
{
    double rr = radians*radians;
    double a = 9.5168091e-03f;
    a *= rr;
    a += 2.900525e-03f;
    a *= rr;
    a += 2.45650893e-02f;
    a *= rr;
    a += 5.33740603e-02f;
    a *= rr;
    a += 1.333923995e-01f;
    a *= rr;
    a += 3.333314036e-01f;
    a *= rr;
    a += 1.0f;
    a *= radians;
    return a;
}
internal double asin_f64(double a)
{
    return atan2_f64(a, sqrt_f64((1.0f + a) * (1.0f - a)));
}
internal double acos_f64(double a)
{
    return atan2_f64(sqrt_f64((1.0f + a) * (1.0 - a)), a);
}
internal double atan_f64(double a)
{
    double u  = a*a;
    double u2 = u*u;
    double u3 = u2*u;
    double u4 = u3*u;
    double f  = 1.0 + 0.33288950512027 * u - 0.08467922817644 * u2 + 0.03252232640125 * u3 - 0.00749305860992 * u4;
    return a / f;
}
internal double atan2_f64(double y, double x)
{
    double result;
    if (Abs(x) > Abs(y))
    {
        double a = atan_f64(y / x);
        if (x > 0.0)
        {
            result = a;
        }
        else
        {
            result = y > 0.0 ? a + TAU_OVER_2_F64 : a - TAU_OVER_2_F64;
        }
    }
    else
    {
        double a = atan_f64(x / y);
        if (x > 0.0)
        {
            result = y > 0.0 ? TAU_OVER_4_F64 - a : -TAU_OVER_4_F64 - a;
        }
        else
        {
            result = y > 0.0 ? TAU_OVER_4_F64 + a : -TAU_OVER_4_F64 + a;
        }
    }
    return result;
}

// Nearest integer floating-point operations ==================================

internal float round_f32(float x)
{
    return (float)((x >= 0.0f) ? floor_f32(x + 0.5f) : ceil_f32(x - 0.5f));
}
internal float floor_f32(float x)
{
    return (float)((x >= 0.0f) ? (int32_t)x : (int32_t)(x-0.9999999999999999f));
}
internal float ceil_f32(float x)
{
    return (float)((x < 0) ? (int32_t)x : ((int32_t)x)+1);
}

internal double round_f64(double x)
{
    return (double)((x >= 0.0) ? (int64_t)(x + 0.5) : (int64_t)(x - 0.5));
}
internal double floor_f64(double x)
{
    return (double)((x >= 0.0) ? (int64_t)x : (int64_t)(x - 0.9999999999999999));
}
internal double ceil_f64(double x)
{
    return (double)((x < 0) ? (int64_t)x : (int64_t)(x + 0.9999999999999999));
}

// Floating-point manipulation functions ======================================

internal float copysign_f32(float x, float y)
{
	int ix, iy;
	ix = *(int *)&x;
	iy = *(int *)&y;
	ix &= 0x7fffffff;
	ix |= iy & 0x80000000;
	return *(float *)&ix;
}

internal double copysign_f64(double x, double y)
{
    int64_t ix, iy;
    ix = *(int64_t *)&x;
    iy = *(int64_t *)&y;
    ix &= 0x7fffffffffffffffLL;
    ix |= iy & 0x8000000000000000LL;
    return *(double *)&ix;
}

// Random-number generation
//=============================================================================

internal uint32_t rand_u32(uint32_t seed)
{
    seed = (seed << 13) ^ seed;
    return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
}

// Vector Ops
//=============================================================================

// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal uint32_t rng1_dim_u32(Rng1_U32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal int32_t rng1_dim_i32(Rng1_I32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal uint64_t rng1_dim_u64(Rng1_U64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal int64_t rng1_dim_i64(Rng1_I64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal float rng1_dim_f32(Rng1_F32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}

// 2 Range (Rectangles) =======================================================

internal Vec2_I16 rng2_dim_i16(Rng2_I16 r)
{
    return (Vec2_I16){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Vec2_I32 rng2_dim_i32(Rng2_I32 r)
{
    return (Vec2_I32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Vec2_I64 rng2_dim_i64(Rng2_I64 r)
{
    return (Vec2_I64){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0
    };
}
internal Vec2_F32 rng2_dim_f32(Rng2_F32 r)
{
    return (Vec2_F32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
