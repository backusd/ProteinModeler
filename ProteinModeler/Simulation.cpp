#include "pch.h"
#include "Simulation.h"



Simulation::Simulation() noexcept :
	m_isPaused(true),
	m_boxMax(3.0f)
{}

size_t Simulation::Add(Element element, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity) noexcept
{
	m_elementTypes.push_back(element);
	m_positions.push_back(position);
	m_velocities.push_back(velocity);

	// return the index of the most recent atom
	return m_elementTypes.size() - 1;
}

void Simulation::Update(const Timer& timer)
{
	WINRT_ASSERT(m_positions.size() == m_velocities.size());
	WINRT_ASSERT(m_positions.size() == m_elementTypes.size());

	float timeDelta = static_cast<float>(timer.GetElapsedSeconds());

	if (m_isPaused)
		return;

	if (timeDelta > 0.1)
		return;

	float radius = 0.0f;
	for (unsigned int iii = 0; iii < m_positions.size(); ++iii)
	{
		radius = AtomicRadii[static_cast<int>(m_elementTypes[iii])];

		m_positions[iii].x += m_velocities[iii].x * timeDelta;
		if (m_positions[iii].x + radius > m_boxMax || m_positions[iii].x - radius < -m_boxMax)
			m_velocities[iii].x *= -1;

		m_positions[iii].y += m_velocities[iii].y * timeDelta;
		if (m_positions[iii].y + radius > m_boxMax || m_positions[iii].y - radius < -m_boxMax)
			m_velocities[iii].y *= -1;

		m_positions[iii].z += m_velocities[iii].z * timeDelta;
		if (m_positions[iii].z + radius > m_boxMax || m_positions[iii].z - radius < -m_boxMax)
			m_velocities[iii].z *= -1;
	}
}