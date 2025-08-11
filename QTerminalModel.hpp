#ifndef __TERMINAL_MODEL_HPP__
#define __TERMINAL_MODEL_HPP__

// Qt Libraries
 #include <QAbstractTableModel>
 #include <QBrush>
 
 // Personal Libraries
#include "OtherClasses.hpp"

class QTerminalModel : public QAbstractTableModel
{
	Q_OBJECT
	
	private:
	
		inline static QString				TimeString(quint32);
		
		std::vector<HexTerminalFile>			logs;
		quint32						period = 0u;
		quint32						numberOfCandidates;
		qreal						takeProfit;
		qreal						threshold;
		
		inline QVariant					retrieveAlignment(const QModelIndex&) const;
		inline QVariant					retrieveBackground(const QModelIndex&) const;
		inline QVariant					retrieveForeground(const QModelIndex&) const;
		inline QVariant					retrieveData(const QModelIndex&) const;
	
	public:
	
		inline QTerminalModel(void);
		
		inline qint32					columnCount(const QModelIndex&) const override;
		inline QVariant					data(const QModelIndex&, qint32) const override;
		inline qint32					process(HexTerminalFile*);
		inline qint32					rowCount(const QModelIndex&) const override;
		inline void					setUp(quint32, quint32, qreal, qreal);
};

QTerminalModel::QTerminalModel(void)
{
	QTerminalModel::logs.reserve(100'000u);
}

qint32 QTerminalModel::columnCount(const QModelIndex&) const
{
	return 14;
}

QVariant QTerminalModel::data(const QModelIndex& index, qint32 role = Qt::DisplayRole) const
{
	switch (role)
	{
		case Qt::TextAlignmentRole:
			return QTerminalModel::retrieveAlignment(index);
		
		case Qt::BackgroundRole:
			return QTerminalModel::retrieveBackground(index);
		
		case Qt::DisplayRole:
			return QTerminalModel::retrieveData(index);
		
		case Qt::ForegroundRole:
			return QTerminalModel::retrieveForeground(index);
	}
	
	return QVariant();
}

qint32 QTerminalModel::process(HexTerminalFile* foo)
{
	const auto size = static_cast<qint32>(QTerminalModel::logs.size());
	QAbstractItemModel::beginInsertRows(QModelIndex(), size, size);
	QTerminalModel::logs.push_back(*foo);
	QAbstractItemModel::endInsertRows();
	return size;
}

QVariant QTerminalModel::retrieveAlignment(const QModelIndex& index) const
{
	const auto column = index.column();
	return (column >= 7 and column != 10 ? QVariant::fromValue(Qt::AlignLeft | Qt::AlignVCenter) : Qt::AlignCenter);
}

QVariant QTerminalModel::retrieveBackground(const QModelIndex& index) const
{
	if (QTerminalModel::logs.empty())
		return QColor(Qt::black);
	
	const auto row = static_cast<quint32>(index.row());
	
	if (QTerminalModel::logs.size() == row)
		return (QTerminalModel::period == 0u or (QTerminalModel::logs.back().realTime/QTerminalModel::period) % 2u == 0u ? QColor(Qt::black) : QColor(30, 30, 30));
	
	const auto& file = QTerminalModel::logs[row];
	const auto column = index.column();
		
	switch (column)
	{
		case 7:
		{
			if (file.ratio != 0u and file.buy70 >= QTerminalModel::takeProfit)
				return QColor(0, 95, 255);
			
			break;
		}
		
		case 8:
		{
			if (file.ratio != 0u and file.buy80 >= QTerminalModel::takeProfit)
				return QColor(0, 95, 255);
			
			break;
		}
		
		case 9:
		{
			if (file.ratio != 0u and file.buy90 >= QTerminalModel::takeProfit)
				return QColor(0, 95, 255);
			
			break;
		}
		
		case 11:
		{
			if (file.ratio != 0u and file.sell70 >= QTerminalModel::takeProfit)
				return QColor(135, 0, 0);
			
			break;
		}
		
		case 12:
		{
			if (file.ratio != 0u and file.sell80 >= QTerminalModel::takeProfit)
				return QColor(135, 0, 0);
			
			break;
		}
		
		case 13:
		{
			if (file.ratio != 0u and file.sell90 >= QTerminalModel::takeProfit)
				return QColor(135, 0, 0);
			
			break;
		}
		
		default:
			break;
	}
	
	return (QTerminalModel::period == 0u or (file.realTime/QTerminalModel::period) % 2u == 0u ? QColor(Qt::black) : QColor(30, 30, 30));
}

QVariant QTerminalModel::retrieveData(const QModelIndex& index) const
{
	const auto row = static_cast<quint32>(index.row());
	
	if (QTerminalModel::logs.size() != row)
	{
		const auto column = index.column();
		const auto& file = QTerminalModel::logs[row];
		
		switch (column)
		{
			case 0:
				return QTerminalModel::TimeString(file.realTime);
			
			case 1:
				return (file.gap < 0.f ? QString::number(file.timeSpot) : QString::number(file.gap));
			
			case 2:
				return QString::number(file.volatility, 'f', 2);
			
			case 4:
				return QString::number(file.worstSimilarity, 'f', 3);
			
			case 5:
				return (file.numberOfSamples < 10u ? '0' + QString::number(file.numberOfSamples) : QString::number(file.numberOfSamples));
			
			case 7:
			{
				if (file.ratio == 0u)
					return file.buy;
				
				
				return QString("[70%] ") + (file.buy70 != 0.f ? QString::number(file.buy70, 'f', 2) : QString("---------"));
			}
			
			case 8:
			{
				if (file.ratio == 0u)
					return QVariant();
				
				
				return QString("[80%] ") + (file.buy80 != 0.f ? QString::number(file.buy80, 'f', 2) : QString("---------"));
			}
			
			case 9:
			{
				if (file.ratio == 0u)
					return QVariant();
				
				
				return QString("[90%] ") + (file.buy90 != 0.f ? QString::number(file.buy90, 'f', 2) : QString("---------"));
			}
			
			case 10:
				return (file.ratio != 0u ? QString::number(file.ratio % 1'000u) : QString());
			
			case 11:
			{
				if (file.ratio == 0u)
					return file.sell;
				
				
				return QString("[70%] ") + (file.sell70 != 0.f ? QString::number(file.sell70, 'f', 2) : QString("---------"));
			}
			
			case 12:
			{
				if (file.ratio == 0u)
					return QVariant();
				
				
				return QString("[80%] ") + (file.sell80 != 0.f ? QString::number(file.sell80, 'f', 2) : QString("---------"));
			}
			
			case 13:
			{
				if (file.ratio == 0u)
					return QVariant();
				
				
				return QString("[90%] ") + (file.sell90 != 0.f ? QString::number(file.sell90, 'f', 2) : QString("---------"));
			}
		}
	}
	
	return QVariant();
}

QVariant QTerminalModel::retrieveForeground(const QModelIndex& index) const
{
	const auto row = static_cast<quint32>(index.row());
	
	if (QTerminalModel::logs.size() != row)
	{
		const auto column = index.column();
		
		if (column < 4 or column == 6)
			return QColor(Qt::white);
		
		const auto& file = QTerminalModel::logs[row];
		
		switch (column)
		{
			case 4:
				return (file.worstSimilarity >= QTerminalModel::threshold ? QColor(135, 215, 0) : QColor(255, 175, 0));
			
			case 5:
				return (file.numberOfSamples >= QTerminalModel::numberOfCandidates ? QColor(135, 215, 0) : QColor(255, 175, 0));
			
			case 7:
				return (file.ratio == 0u or file.buy70 < QTerminalModel::takeProfit ? QColor(95, 215, 255) : QColor(Qt::white));
			
			case 8:
				return (file.ratio == 0u or file.buy80 < QTerminalModel::takeProfit ? QColor(95, 215, 255) : QColor(Qt::white));
			
			case 9:
				return (file.ratio == 0u or file.buy90 < QTerminalModel::takeProfit ? QColor(95, 215, 255) : QColor(Qt::white));
			
			case 10:
			{
				switch (file.ratio/1'000u)
				{
					case 1u:
						return QColor(95, 215, 255);
					
					case 2u:
						return QColor(188, 188, 188);
					
					case 3u:
						return QColor(255, 95, 0);
				}
				
				break;
			}
			
			case 11:
				return (file.ratio == 0u or file.sell70 < QTerminalModel::takeProfit ? QColor(255, 95, 0) : QColor(Qt::white));
			
			case 12:
				return (file.ratio == 0u or file.sell80 < QTerminalModel::takeProfit ? QColor(255, 95, 0) : QColor(Qt::white));
			
			case 13:
				return (file.ratio == 0u or file.sell90 < QTerminalModel::takeProfit ? QColor(255, 95, 0) : QColor(Qt::white));
		}
	}
	
	return QVariant();
}

qint32 QTerminalModel::rowCount(const QModelIndex&) const
{
	return static_cast<qint32>(QTerminalModel::logs.size()) + 1;
}

void QTerminalModel::setUp(quint32 p, quint32 nc, qreal th, qreal tp)
{
	QTerminalModel::logs.clear();

	QTerminalModel::period = p;
	QTerminalModel::numberOfCandidates = nc;
	QTerminalModel::threshold = static_cast<qreal>(th);
	QTerminalModel::takeProfit = static_cast<qreal>(tp);
}

QString QTerminalModel::TimeString(quint32 timestamp)
{
	const auto hour = 15u + (timestamp + 1'800u)/3'600u;
	const auto minute = (timestamp/60u + 30u) % 60u;
	const auto second = timestamp % 60u;
	
	const auto zeroPadding1 = (minute < 10u ? QString("0") : QString());
	const auto zeroPadding2 = (second < 10u ? QString("0") : QString());
	return QString::number(hour) + ':' + zeroPadding1 + QString::number(minute) + ':' + zeroPadding2 + QString::number(second);
}

#endif
