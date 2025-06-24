#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSet>

class Jugador;

/* ─────────────────────────────────────────────────────────
 *  SkillTreeWidget
 *  Árbol binario de 7 nodos con iconos.  Permite gastar
 *  puntos y aplicar mejoras directamente al Jugador.
 * ───────────────────────────────────────────────────────── */
class SkillTreeWidget : public QWidget
{
    Q_OBJECT
public:
    /* TOTAL: 7 botones  (0–6)  */
    static constexpr int NODE_COUNT = 7;

    /* ctor: se puede pasar el jugador directamente  */
    explicit SkillTreeWidget(Jugador *player, QWidget *parent = nullptr);

    /* ——— API pública ——— */
    void addPoints(int n);                 ///< sumar puntos
    int  points() const { return m_points; }

    /*  *FIX*  -> permite asignar/actualizar el jugador
     *  (necesario si lo creas antes de instanciar al Jugador)  */
    void setPlayer(Jugador *p) { m_player = p; }

signals:
    void superJump1Unlocked();             ///< notifica a MainWindow

protected:
    void showEvent (QShowEvent *ev) override;   ///< centrado auto
    void paintEvent(QPaintEvent *ev) override;  ///< fondo + líneas

private slots:
    void onNodeClicked();                      ///< compra de nodo

private:
    /* helpers internos */
    void initLayout();                 ///< crea y posiciona botones
    void refreshPointsLabel();         ///< “Puntos: n”
    void unlockChildren(int idx);      ///< habilita hijos binarios
    void applyEffect   (int idx);      ///< aplica mejora al jugador

    /* ─── datos ─── */
    QPushButton* m_nodes[NODE_COUNT]{};  ///< botones 0-6
    QLabel      *m_pointsLbl   = nullptr;
    QGridLayout *m_grid        = nullptr;   ///< acceso directo al grid

    QSet<int>    m_unlocked;     ///< índices ya comprados
    int          m_points   = 0; ///< puntos disponibles
    Jugador*     m_player   = nullptr;  ///< para aplicar mejoras
};
