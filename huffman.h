#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <QChar>
#include <QString>
#include <QHash>
#include <QStack>
#include <QVector>
#include <QTextStream>
#include <QByteArray>
#include <queue>
#include <vector>

class Huffman
{
private:
    struct Node
    {
        QChar ch;
        int weight;
        int child[2];
        Node(): ch('\0'), weight(0)
        {
            child[0]=0;
            child[1]=0;
        }
        Node(QChar ch, int weight): ch(ch), weight(weight)
        {
            child[0]=0;
            child[1]=0;
        }
        Node(int n1, int n2): ch('\0'), weight(tree[n1].weight + tree[n2].weight)
        {
            child[0]=n1;
            child[1]=n2;
        }
    };
    struct cmp
    {
        bool operator() (int n1, int n2) const
        {
            return tree[n1].weight > tree[n2].weight;
        }
    };
    static int con;
    static QVector<Node> tree;
public:
    static void createTree(const QString& plain);
    static void treeToMap(int n, QString pre);
    static void restoreTree(QString& treeStruct);
    static void restoreTree(uchar* treeStruct, int treesize);
    static QString encode(const QString& str);
    static QString decode(const QString& str);
    static QString treeStruct();
    static QByteArray binTreeStruct();
    static QHash<QChar, QString> mapping;
    ~Huffman();
};

#endif // HUFFMAN_H
