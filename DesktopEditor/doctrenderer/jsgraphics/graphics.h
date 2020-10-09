#ifndef CJSGRAPHICS_H
#define CJSGRAPHICS_H

#include "../../common/Types.h"
#include "../../../Common/3dParty/v8/v8/include/v8.h"
#include "../../../Common/3dParty/v8/v8/include/libplatform/libplatform.h"

class CJSGraphics
{
private:
    v8::Local<v8::Value> m_oContext;
    double m_dWidthMM;
    double m_dHeightMM;
    double m_lWidthPix;
    double m_lHeightPix;
    double m_dDpiX;
    double m_dDpiY;

    v8::Local<v8::Value> m_oPen;
    bool m_bPenColorInit;
    v8::Local<v8::Value> m_oBrush;
    bool m_bBrushColorInit;

    v8::Local<v8::Value>* m_oFontManager;

    v8::Local<v8::Value> m_oCoordTransform;
    v8::Local<v8::Value> m_oTransform;
    v8::Local<v8::Value> m_oFullTransform;
    v8::Local<v8::Value> m_oInvertFullTransform;

    v8::Local<v8::Value>* ArrayPoints;

    v8::Local<v8::Value> m_oLastFont;

    bool m_bIntegerGrid;

    int TextureFillTransformScaleX;
    int TextureFillTransformScaleY;

    int globalAlpha = 1;

    v8::Local<v8::Value> dash_no_smart;
public:
    CJSGraphics();

    void init(const v8::Local<v8::Value>& context, double width_px, double height_px, double width_mm, double height_mm);
    void EndDraw() {}
    void put_GlobalAlpha(bool enable, int globalAlpha);
    void Start_GlobalAlpha() {}
    void End_GlobalAlpha();
    // pen methods
    void p_color(int r, int g, int b, int a);
    void p_width(int w);
    void p_dash(const v8::Local<v8::Value>& params);
    // brush methods
    void b_color1(int r, int g, int b, int a);
    void b_color2(int r, int g, int b, int a);
    void transform(double sx, double shy, double shx, double sy, double tx, double ty);
    void CalculateFullTransform(bool isInvertNeed);
    // path commands
    void _s();
    void _e();
    void _z();
    void _m(double x, double y);
    void _l(double x, double y);
    void _c(double x1, double y1, double x2, double y2, double x3, double y3);
    void _c2(double x1, double y1, double x2, double y2);
    void _ds();
    void _df();
    // canvas state
    void save();
    void restore();
    void clip();
    void reset();
    void transform3(const v8::Local<v8::Value>& m, bool isNeedInvert);
    void FreeFont();
    void ClearLastFont();

    double m_dDpiX_get() { return m_dDpiX; }
    int globalAlpha_get() { return globalAlpha; }
};

#endif // CJSGRAPHICS_H
