#include "huffman.h"
#include <QDebug>

QVector<Huffman::Node> Huffman::tree;
QHash<QChar, QString> Huffman::mapping;
int Huffman::con;

Huffman::~Huffman(){}

void Huffman::createTree(const QString &plain)
{
    if(plain.isEmpty())
    {
        tree.clear();
        return;
    }
    QHash<QChar, int> countOfChar;
    std::priority_queue<int, std::vector<int>, cmp> que;
    for(const QChar ch : plain)
        countOfChar[ch]++;
    if(countOfChar.size()==1)
    {
        tree.resize(2);
        tree[0].ch='\0';
        tree[0].child[0]=1;
        tree[0].child[1]=0;
        tree[1].ch=countOfChar.begin().key();
        return;
    }
    con=1;
    tree.resize(countOfChar.size()*2-1);
    QHashIterator<QChar, int> it(countOfChar);
    while(it.hasNext())
    {
        it.next();
        tree[con]=Node(it.key(), it.value());
        que.push(con);
        con++;
    }
    while(que.size()>2)
    {
        int n1 = que.top();
        que.pop();
        int n2 = que.top();
        que.pop();
        tree[con]=Node(n1, n2);
        que.push(con);
        con++;
    }
    int n1 = que.top();
    que.pop();
    int n2 = que.top();
    tree[0]=Node(n1, n2);
}

void Huffman::treeToMap(int n, QString pre)
{
    if(tree.empty())
    {
        mapping.clear();
        return;
    }
    if(tree[n].ch!='\0')
        mapping.insert(tree[n].ch, pre);
    else
    {
        if(tree[n].child[0])
            treeToMap(tree[n].child[0], pre+'0');
        if(tree[n].child[1])
            treeToMap(tree[n].child[1], pre+'1');
    }
}

QString Huffman::encode(const QString &str)
{
    createTree(str);
    mapping.clear();
    treeToMap(0, QString());
    QString rst;
    for(const QChar ch : str)
        rst+=mapping[ch];
    return rst;
}

void Huffman::restoreTree(QString &treeStruct)
{
    QTextStream ss(&treeStruct);
    ss >> con;
    tree.resize(con);
    for(int i=0; i<con; i++)
    {
        ushort ch;
        ss >> ch >> tree[i].child[0] >> tree[i].child[1];
        tree[i].ch = QChar(ch);
    }
}

void Huffman::restoreTree(uchar* treeStruct, int treesize)
{
    QByteArray newTree = qUncompress(treeStruct, treesize);
    con=*(int*)newTree.data();
    tree.resize(con);
    size_t p = sizeof(int);
    for(int i=0; i<con; i++)
    {
        qDebug() << "p=" << p;
        ushort ch = *(ushort*)(newTree.data()+p);
        tree[i].ch = QChar(ch);
        qDebug() << "ch=" <<  tree[i].ch;
        p+=sizeof(ushort);
        tree[i].child[0] = *(int*)(newTree.data()+p);
        p+=sizeof(int);
        tree[i].child[1] = *(int*)(newTree.data()+p);
        p+=sizeof(int);
    }
}

QString Huffman::decode(const QString &str)
{
    int p=0;
    QString rst;
    for(const QChar ch : str)
    {
        p=tree[p].child[ch.digitValue()];
        if(p==0)
            return rst;
        if(tree[p].ch!='\0')
        {
            rst+=tree[p].ch;
            p=0;
        }
    }
    return rst;
}

QString Huffman::treeStruct()
{
    QString rst;
    rst+=QString::number(con);
    for(int i=0; i<con; i++)
    {
        rst+= QString(" ") + QString::number(tree[i].ch.unicode()) + QString(" ") + QString::number(tree[i].child[0]) + QString(" ") + QString::number(tree[i].child[1]);
    }
    return rst;
}

QByteArray Huffman::binTreeStruct()
{
    QByteArray rst;
    rst.append((char*)&con, sizeof(int));
    for(int i=0; i<con; i++)
    {
        rst.append((char*)&tree[i].ch.unicode(), sizeof(ushort));
        rst.append((char*)&tree[i].child[0], sizeof(int));
        rst.append((char*)&tree[i].child[1], sizeof(int));
    }
    rst = qCompress(rst, 9);
    return rst;
}
