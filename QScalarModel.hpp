#ifndef __SCALAR_MODEL_HPP__
#define __SCALAR_MODEL_HPP__

// Qt Libraries
#include <QAbstractTableModel>
#include <QBrush> 

// Personal Libraries
#include "OtherClasses.hpp"

class QScalarModel : public QAbstractTableModel
{
	Q_OBJECT
	
	private:
	
		const std::vector<HexFullFile>&		precedents;
		
		inline QVariant				horizontalHeader(qint32) const;
		inline QVariant				retrieveBackground(const QModelIndex&) const;
		inline QVariant				retrieveData(const QModelIndex&) const;
		inline QVariant				verticalHeader(qint32) const;
	
	public:
	
		inline					QScalarModel(const std::vector<HexFullFile>&);
		inline qint32				columnCount(const QModelIndex&) const override;
		inline QVariant				data(const QModelIndex&, qint32) const override;
		inline QVariant				headerData(qint32, Qt::Orientation, qint32) const override;
		inline qint32				rowCount(const QModelIndex&) const override;
};

QScalarModel::QScalarModel(const std::vector<HexFullFile>& foo) : precedents(foo)
{
}

qint32 QScalarModel::columnCount(const QModelIndex&) const
{
	return static_cast<qint32>(QScalarModel::precedents.size());
}

QVariant QScalarModel::data(const QModelIndex& index, qint32 role) const
{
	switch (role)
	{
		case Qt::BackgroundRole:
			return QScalarModel::retrieveBackground(index);
		
		case Qt::DisplayRole:
			return QScalarModel::retrieveData(index);
		
		case Qt::TextAlignmentRole:
			return Qt::AlignCenter;
	}
	
	return QVariant();
}

QVariant QScalarModel::headerData(qint32 nb, Qt::Orientation ort, qint32 role) const
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;
	else if (role != Qt::DisplayRole)
		return QVariant();
	
	switch (ort)
	{
		case Qt::Horizontal:
			return QScalarModel::horizontalHeader(nb);
		
		case Qt::Vertical:
			return QScalarModel::verticalHeader(nb);
	}
	
	return QVariant();
}

QVariant QScalarModel::horizontalHeader(qint32 column) const
{
	return QString::number(column + 1);
}

QVariant QScalarModel::retrieveBackground(const QModelIndex& index) const
{
	const auto column = index.column();
	const auto& file = QScalarModel::precedents[static_cast<quint32>(column)];
	
	switch (file.foo)
	{
		case 'b':
			return QBrush(QColor(102, 153, 255));
		
		case 'e':
		{
			if (file.bestBuyProfit >= 2.*file.bestSellProfit)
				return QBrush(QColor(102, 153, 255), Qt::Dense4Pattern);
			
			if (file.bestSellProfit >= 2.*file.bestBuyProfit)
				return QBrush(QColor(255, 128, 128), Qt::Dense4Pattern);
			
			return QBrush(QColor(153, 153, 255), Qt::Dense4Pattern);
		}
		
		case 's':
			return QBrush(QColor(255, 128, 128));
		
		case 'u':
			return QBrush(QColor(204, 204, 204));
	}
	
	return QVariant();
}

QVariant QScalarModel::retrieveData(const QModelIndex& index) const
{
	const auto column = index.column();
	const auto row = index.row();
	const auto& file = QScalarModel::precedents[static_cast<quint32>(column)];
	
	switch (row)
	{
		case 0:
			return file.date;
		
		case 1:
			return file.time;
		
		case 2:
			return QString::number(file.similarity*100., 'f', 3);
		
		case 3:
			return "× " + QString::number(file.factor, 'f', 3);
		
		case 4:
			return QString::number(file.bestBuyProfit, 'g');
		
		case 5:
			return QString::number(file.bestSellProfit, 'g');
	}
	
	return QVariant();
}

qint32 QScalarModel::rowCount(const QModelIndex&) const
{
	return (QScalarModel::precedents.empty() ? 0 : 6);
}

QVariant QScalarModel::verticalHeader(qint32 row) const
{
	switch (row)
	{
		case 0:
			return QString("Date");
		
		case 1:
			return QString("Time");
		
		case 2:
			return QString("Mark");

		case 3:
			return QString("Scale");
		
		case 4:
			return QString("Buy");
		
		case 5:
			return QString("Sell");
	}
	
	return QVariant();
}

#endif
