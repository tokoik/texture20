#if defined(__APPLE__) || defined(MACOSX)
#  define GL_SILENCE_DEPRECATION
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#  include <GL/glext.h>
#  if defined(_MSC_VER)
extern PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;
#  endif
#endif

#include "rectangle.h"
#include "matrix.h"

/*
** 矩形の描画
*/
void rectangle(double w, double h, const float l[])
{
  /* 頂点の座標値 */
  const GLdouble vertex[4][3] = {
    { -w, -h, 0.0 },
    {  w, -h, 0.0 },
    {  w,  h, 0.0 },
    { -w,  h, 0.0 }
  };
  
  /* 頂点のテクスチャ座標 */
  static const GLdouble texcoord[4][2] = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }
  };
  
  /* 現在のモデルビュー変換行列の逆行列を求める */
  double m[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, m);
  inverse(m, m);

  /* 接空間（ローカル座標系）における光源位置を求める */
  double lpos[4] = { l[0], l[1], l[2], l[3] };
  transform(lpos, m, lpos);
  
  /* 平行光線でなければ実座標を求めておく */
  if (lpos[3] != 0.0) {
    lpos[0] /= lpos[3];
    lpos[1] /= lpos[3];
    lpos[2] /= lpos[3];
  }
  
  /* 矩形を描く */
  glBegin(GL_QUADS);
  
  for (int i = 0; i < 4; ++i) {

    /* 法線マップのテクスチャ座標を設定する */
    glMultiTexCoord2dv(GL_TEXTURE0, texcoord[i]);

    /* 拡散反射率マップのテクスチャ座標を設定する */
    glMultiTexCoord2dv(GL_TEXTURE2, texcoord[i]);
    
    /* 接空間における光源の方向ベクトルを
       正規化マップのテクスチャ座標に設定する */
    if (lpos[3] != 0.0) {
      glMultiTexCoord3d(GL_TEXTURE1,
        lpos[0] - vertex[i][0],
        lpos[1] - vertex[i][1],
        lpos[2] - vertex[i][2]);
    }
    else {
      glMultiTexCoord3d(GL_TEXTURE1, lpos[0], lpos[1], lpos[2]);
    }
    
    /* 対応する頂点座標の指定 */
    glVertex3dv(vertex[i]);
  }
  glEnd();
}
